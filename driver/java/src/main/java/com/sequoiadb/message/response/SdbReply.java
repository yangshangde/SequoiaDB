/*
 * Copyright 2018 SequoiaDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.sequoiadb.message.response;

import com.sequoiadb.message.ResultSet;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

import java.nio.ByteBuffer;

public class SdbReply extends CommonResponse {
    private ResultSet resultSet;

    public ResultSet getResultSet() {
        return resultSet;
    }

    public BSONObject getReturnData(){
        BSONObject result = null;
        if (resultSet != null && resultSet.hasNext()) {
            result = resultSet.getNext();
        }
        return result;
    }

    @Override
    protected void decodeData(ByteBuffer in) {
        if (flag == 0 && in.hasRemaining()) {
            resultSet = new ResultSet(in, returnedNum);
        }
    }
}
