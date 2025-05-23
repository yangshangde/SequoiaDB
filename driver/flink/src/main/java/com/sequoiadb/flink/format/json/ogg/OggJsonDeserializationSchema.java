/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.sequoiadb.flink.format.json.ogg;

import com.sequoiadb.flink.common.exception.SDBException;
import com.sequoiadb.flink.common.metadata.ExtraRowKind;
import org.apache.flink.annotation.Internal;
import org.apache.flink.api.common.serialization.DeserializationSchema;
import org.apache.flink.api.common.typeinfo.TypeInformation;
import org.apache.flink.formats.common.TimestampFormat;
import org.apache.flink.formats.json.JsonRowDataDeserializationSchema;
import org.apache.flink.table.api.DataTypes;
import org.apache.flink.table.data.GenericRowData;
import org.apache.flink.table.data.RowData;
import org.apache.flink.table.types.DataType;
import org.apache.flink.table.types.logical.RowType;
import org.apache.flink.table.types.utils.DataTypeUtils;
import org.apache.flink.types.RowKind;
import org.apache.flink.util.Collector;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.io.Serializable;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.stream.Collectors;

import static java.lang.String.format;
import static com.sequoiadb.flink.format.json.ogg.OggJsonDecodingFormat.ReadableMetadata;

/**
 * Deserialization schema from Ogg JSON to Flink Table/SQL internal data structure {@link RowData}.
 * The deserialization schema knows Ogg's schema definition and can extract the database data and
 * convert into {@link RowData} with {@link RowKind}.
 *
 * <p>Deserializes a <code>byte[]</code> message as a JSON object and reads the specified fields.
 *
 * <p>Failures during deserialization are forwarded as wrapped IOExceptions.
 *
 * @see <a href="https://www.oracle.com/cn/middleware/technologies/goldengate/overview.html">Ogg</a>
 */
@Internal
public final class OggJsonDeserializationSchema implements DeserializationSchema<RowData> {

    private static final Logger LOG = LoggerFactory.getLogger(OggJsonDeserializationSchema.class);

    private static final long serialVersionUID = 1L;

    private static final Set<String> SUPPORTED_CHANGELOG_PARTITION_POLICIES = new HashSet<>();

    private static final String OP_CREATE = "I"; // insert
    private static final String OP_UPDATE = "U"; // update
    private static final String OP_DELETE = "D"; // delete
    private static final String OP_TRUNCATE = "T"; // truncate

    private static final String P_BY_BEF = "p-by-bef";
    private static final String P_BY_AFT = "p-by-aft";

    private static final String REPLICA_IDENTITY_EXCEPTION =
            "The \"before\" field of %s message is null, "
                    + "if you are using Ogg Postgres Connector, "
                    + "please check the Postgres table has been set REPLICA IDENTITY to FULL level.";

    static {
        SUPPORTED_CHANGELOG_PARTITION_POLICIES.add(P_BY_BEF);
        SUPPORTED_CHANGELOG_PARTITION_POLICIES.add(P_BY_AFT);
    }

    /** The deserializer to deserialize Ogg JSON data. */
    private final JsonRowDataDeserializationSchema jsonDeserializer;

    /** Flag that indicates that an additional projection is required for metadata. */
    private final boolean hasMetadata;

    /** Metadata to be extracted for every record. */
    private final MetadataConverter[] metadataConverters;

    /** {@link TypeInformation} of the produced {@link RowData} (physical + metadata). */
    private final TypeInformation<RowData> producedTypeInfo;

    /** Flag indicating whether to ignore invalid fields/rows (default: throw an exception). */
    private final boolean ignoreParseErrors;
    private final int[] upsertKeyPositions;
    private final String cPartitionPolicy;

    private final int erkPos;
    private final RowType jsonRowType;

    public OggJsonDeserializationSchema(
            DataType physicalDataType,
            List<ReadableMetadata> requestedMetadata,
            TypeInformation<RowData> producedTypeInfo,
            boolean ignoreParseErrors,
            TimestampFormat timestampFormat,
            String[] upsertKey,
            String cPartitionPolicy) {
        this.jsonRowType = createJsonRowType(physicalDataType, requestedMetadata);
        this.jsonDeserializer =
                new JsonRowDataDeserializationSchema(
                        jsonRowType,
                        // the result type is never used, so it's fine to pass in the produced type
                        // info
                        producedTypeInfo,
                        false, // ignoreParseErrors already contains the functionality of
                        // failOnMissingField
                        ignoreParseErrors,
                        timestampFormat);
        this.hasMetadata = requestedMetadata.size() > 0;
        this.metadataConverters = createMetadataConverters(jsonRowType, requestedMetadata);
        this.producedTypeInfo = producedTypeInfo;
        this.ignoreParseErrors = ignoreParseErrors;

        final RowType physicalRowType = (RowType) physicalDataType.getLogicalType();
        this.upsertKeyPositions = new int[upsertKey.length];
        for (int i = 0; i < upsertKey.length; i++) {
            int pos = findFieldPosByName(upsertKey[i], physicalRowType);
            if (pos == -1) {
                throw new SDBException(
                        String.format("can't match primary key: %s in schema: %s.",
                                upsertKey[i],
                                physicalRowType));
            }
            upsertKeyPositions[i] = pos;
        }

        this.cPartitionPolicy = cPartitionPolicy;
        if (!SUPPORTED_CHANGELOG_PARTITION_POLICIES.contains(cPartitionPolicy)) {
            throw new SDBException(
                    String.format("unrecognized changelog partition policy: %s",
                            cPartitionPolicy));
        }

        this.erkPos = requestedMetadata
                .indexOf(ReadableMetadata.EXTRA_ROW_KIND);
    }

    private static RowType createJsonRowType(
            DataType physicalDataType, List<ReadableMetadata> readableMetadata) {
        DataType root =
                DataTypes.ROW(
                        DataTypes.FIELD("before", physicalDataType),
                        DataTypes.FIELD("after", physicalDataType),
                        DataTypes.FIELD("op_type", DataTypes.STRING()));
        // append fields that are required for reading metadata in the root
        final List<DataTypes.Field> rootMetadataFields =
                readableMetadata.stream()
                        .map(m -> m.requiredJsonField)
                        .filter(m -> !m.getName().equals("op_type"))
                        .distinct()
                        .collect(Collectors.toList());
        return (RowType) DataTypeUtils.appendRowFields(root, rootMetadataFields).getLogicalType();
    }

    private static MetadataConverter[] createMetadataConverters(
            RowType jsonRowType, List<ReadableMetadata> requestedMetadata) {
        return requestedMetadata.stream()
                .map(m -> convertInRoot(jsonRowType, m))
                .toArray(MetadataConverter[]::new);
    }

    private static MetadataConverter convertInRoot(RowType jsonRowType, ReadableMetadata metadata) {
        final int pos = findFieldPos(metadata, jsonRowType);
        return new MetadataConverter() {
            private static final long serialVersionUID = 1L;

            @Override
            public Object convert(GenericRowData root, int unused) {
                return metadata.converter.convert(root, pos);
            }
        };
    }

    private static int findFieldPos(ReadableMetadata metadata, RowType jsonRowType) {
        final int pos = jsonRowType
                .getFieldNames()
                .indexOf(metadata.requiredJsonField.getName());
        if (pos < 0) {
            return -1;
        }
        return pos;
    }

    @Override
    public RowData deserialize(byte[] message) {
        throw new RuntimeException(
                "Please invoke DeserializationSchema#deserialize(byte[], Collector<RowData>) instead.");
    }

    @Override
    public void deserialize(byte[] message, Collector<RowData> out) throws IOException {
        if (message == null || message.length == 0) {
            // skip tombstone messages
            return;
        }

        try {
            GenericRowData row = (GenericRowData) jsonDeserializer.deserialize(message);

            GenericRowData before = (GenericRowData) row.getField(0);
            GenericRowData after = (GenericRowData) row.getField(1);
            String op = row.getField(2).toString();

            if (OP_CREATE.equals(op)) {
                after.setRowKind(RowKind.INSERT);
                emitRow(row, after, out);
            } else if (OP_UPDATE.equals(op)) {
                if (before == null) {
                    throw new IllegalStateException(
                            String.format(REPLICA_IDENTITY_EXCEPTION, "UPDATE"));
                }
                before.setRowKind(RowKind.UPDATE_BEFORE);
                after.setRowKind(RowKind.UPDATE_AFTER);
                emitRow(row, before, after, out);
            } else if (OP_DELETE.equals(op)) {
                if (before == null) {
                    throw new IllegalStateException(
                            String.format(REPLICA_IDENTITY_EXCEPTION, "DELETE"));
                }
                before.setRowKind(RowKind.DELETE);
                emitRow(row, before, out);
            } else {
                if (!ignoreParseErrors) {
                    throw new IOException(
                            format("Unknown \"op_type\" value \"%s\".", op));
                }
            }
        } catch (Throwable t) {
            // a big try catch to protect the processing.
            if (!ignoreParseErrors) {
                throw new IOException(String.format("Corrupt Ogg JSON message. %s", new String(message)), t);
            }
        }
    }

    // --------------------------------------------------------------------------------------------

    private static int findFieldPosByName(String fieldName, RowType physicalRowType) {
        return physicalRowType.getFieldNames().indexOf(fieldName);
    }

    private boolean hasPriKeyChanges(
            GenericRowData before, GenericRowData after) {
        for (int upsertKeyPosition : upsertKeyPositions) {
            if (!before.isNullAt(upsertKeyPosition) && !after.isNullAt(upsertKeyPosition)) {
                Object field1 = before.getField(upsertKeyPosition);
                Object field2 = after.getField(upsertKeyPosition);
                if (!field1.equals(field2)) {
                    return true;
                }
            }
        }
        return false;
    }

    private void emitRow(GenericRowData rootRow,
            GenericRowData before, GenericRowData after, Collector<RowData> out) throws IOException {
        GenericRowData producedAft = convertToFinalRow(rootRow, after);

        // if it has primary key changes, split changelog into UPDATE_PK_BEF, UPDATE_PK_AFT
        if (hasPriKeyChanges(before, after)) {
            GenericRowData producedBef = convertToFinalRow(rootRow, before);

            /**
             * Set $extra-row-kind if metadata has been defined (erkPos == -1 means
             * It hasn't been defined).
             *
             * Currently, $extra-row-kind is only provided to the
             * {@link com.sequoiadb.flink.sink.writer.SDBPartitionedSinkWriter} for use, and upstream
             * does not need to check if it is defined.
             */
            if (erkPos != -1) {
                final int physicalArity = after.getArity();
                producedBef.setField(erkPos + physicalArity, ExtraRowKind.UPDATE_PK_BEF.getCode());
                producedAft.setField(erkPos + physicalArity, ExtraRowKind.UPDATE_PK_AFT.getCode());

                if (P_BY_BEF.equals(cPartitionPolicy)) {
                    // if update changelog is partitioned by primary-keys in before field,
                    // UPDATE_PK_BEF is still in current pipeline, turn into a DELETE op.
                    producedBef.setRowKind(RowKind.DELETE);
                    producedBef.setField(erkPos + physicalArity, ExtraRowKind.DELETE.getCode());
                } else if (P_BY_AFT.equals(cPartitionPolicy)) {
                    // if update changelog is partitioned by primary-keys in after field,
                    // UPDATE_PK_AFT is still in current pipeline, turn into an INSERT op.
                    producedAft.setRowKind(RowKind.INSERT);
                    producedAft.setField(erkPos + physicalArity, ExtraRowKind.INSERT.getCode());
                }
            }
            out.collect(producedBef);
        }

        out.collect(producedAft);
    }

    private GenericRowData convertToFinalRow(GenericRowData rootRow, GenericRowData physicalRow) {
        // shortcut in case no output projection is required
        if (!hasMetadata) {
            return physicalRow;
        }

        final int physicalArity = physicalRow.getArity();
        final int metadataArity = metadataConverters.length;

        final GenericRowData producedRow =
                new GenericRowData(physicalRow.getRowKind(), physicalArity + metadataArity);

        for (int physicalPos = 0; physicalPos < physicalArity; physicalPos++) {
            producedRow.setField(physicalPos, physicalRow.getField(physicalPos));
        }

        for (int metadataPos = 0; metadataPos < metadataArity; metadataPos++) {
            producedRow.setField(
                    physicalArity + metadataPos, metadataConverters[metadataPos].convert(rootRow));
        }

        return producedRow;
    }

    private void emitRow(
            GenericRowData rootRow, GenericRowData physicalRow, Collector<RowData> out) {
        out.collect(convertToFinalRow(rootRow, physicalRow));
    }

    @Override
    public boolean isEndOfStream(RowData nextElement) {
        return false;
    }

    @Override
    public TypeInformation<RowData> getProducedType() {
        return producedTypeInfo;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        OggJsonDeserializationSchema that = (OggJsonDeserializationSchema) o;
        return Objects.equals(jsonDeserializer, that.jsonDeserializer)
                && hasMetadata == that.hasMetadata
                && Objects.equals(producedTypeInfo, that.producedTypeInfo)
                && ignoreParseErrors == that.ignoreParseErrors;
    }

    @Override
    public int hashCode() {
        return Objects.hash(jsonDeserializer, hasMetadata, producedTypeInfo, ignoreParseErrors);
    }

    // --------------------------------------------------------------------------------------------

    /**
     * Converter that extracts a metadata field from the row (root or payload) that comes out of the
     * JSON schema and converts it to the desired data type.
     */
    interface MetadataConverter extends Serializable {

        // Method for top-level access.
        default Object convert(GenericRowData row) {
            return convert(row, -1);
        }

        Object convert(GenericRowData row, int pos);
    }
}
