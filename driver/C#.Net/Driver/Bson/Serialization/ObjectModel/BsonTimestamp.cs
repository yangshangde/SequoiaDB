﻿/* Copyright 2010-2012 10gen Inc.
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

namespace SequoiaDB.Bson
{
    /// <summary>
    /// Represents a BSON timestamp value.
    /// </summary>
    [Serializable]
    public class BsonTimestamp : BsonValue, IComparable<BsonTimestamp>, IEquatable<BsonTimestamp>
    {
        // private fields
        private long _value;

        // constructors
        /// <summary>
        /// Initializes a new instance of the BsonTimestamp class.
        /// </summary>
        /// <param name="value">The combined timestamp/increment value. 
        /// e.g. when timstamp is "2147483647s", 
        /// and the increment is "123456us"(both the timestamp and the increment are should be int32 values,
        /// besides, increment should in the range of [0us, 999999us]),
        /// the value = (long)(((ulong)(uint)2147483647 << 32) | ((ulong)(uint)123456)).</param>
        public BsonTimestamp(long value)
            : base(BsonType.Timestamp)
        {
            _value = value;
        }

        /// <summary>
        /// Initializes a new instance of the BsonTimestamp class.
        /// </summary>
        /// <param name="timestamp">The seconds since epoch.</param>
        /// <param name="increment">The microseconds in range of [0us, 999999us], 
        ///                         while the 'inc' is out of range, the carry will occur.</param>
        public BsonTimestamp(int timestamp, int increment)
            : base(BsonType.Timestamp)
        {
            if (increment < 0 || increment >= 1000000)
            {
                timestamp += increment / 1000000;
                increment = increment % 1000000;
                if (increment < 0)
                {
                    timestamp -= 1;
                    increment += 1000000;
                }
            }
            _value = (long)(((ulong)(uint)timestamp << 32) | ((ulong)(uint)increment));
        }

        // public operators
        /// <summary>
        /// Compares two BsonTimestamp values.
        /// </summary>
        /// <param name="lhs">The first BsonTimestamp.</param>
        /// <param name="rhs">The other BsonTimestamp.</param>
        /// <returns>True if the two BsonTimestamp values are not equal according to ==.</returns>
        public static bool operator !=(BsonTimestamp lhs, BsonTimestamp rhs)
        {
            return !(lhs == rhs);
        }

        /// <summary>
        /// Compares two BsonTimestamp values.
        /// </summary>
        /// <param name="lhs">The first BsonTimestamp.</param>
        /// <param name="rhs">The other BsonTimestamp.</param>
        /// <returns>True if the two BsonTimestamp values are equal according to ==.</returns>
        public static bool operator ==(BsonTimestamp lhs, BsonTimestamp rhs)
        {
            if (object.ReferenceEquals(lhs, null)) { return object.ReferenceEquals(rhs, null); }
            return lhs.Equals(rhs);
        }

        // public properties
        /// <summary>
        /// Gets the value of this BsonTimestamp.
        /// </summary>
        public long Value
        {
            get { return _value; }
        }

        /// <summary>
        /// Gets the increment.
        /// </summary>
        public int Increment
        {
            get { return (int)_value; }
        }

        /// <summary>
        /// Gets the timestamp.
        /// </summary>
        public int Timestamp
        {
            get { return (int)(_value >> 32); }
        }

        // public static methods
        /// <summary>
        /// Creates a new instance of the BsonTimestamp class.
        /// </summary>
        /// <param name="value">The combined timestamp/increment value.</param>
        /// <returns>A BsonTimestamp.</returns>
        public static BsonTimestamp Create(long value)
        {
            return new BsonTimestamp(value);
        }

        /// <summary>
        /// Creates a new instance of the BsonTimestamp class.
        /// </summary>
        /// <param name="timestamp">The timestamp.</param>
        /// <param name="increment">The increment.</param>
        /// <returns>A BsonTimestamp.</returns>
        public static BsonTimestamp Create(int timestamp, int increment)
        {
            return new BsonTimestamp(timestamp, increment);
        }

        /// <summary>
        /// Creates a new BsonTimestamp.
        /// </summary>
        /// <param name="value">An object to be mapped to a BsonTimestamp.</param>
        /// <returns>A BsonTimestamp or null.</returns>
        public new static BsonTimestamp Create(object value)
        {
            if (value != null)
            {
                return (BsonTimestamp)BsonTypeMapper.MapToBsonValue(value, BsonType.Timestamp);
            }
            else
            {
                return null;
            }
        }

        // public methods
        /// <summary>
        /// Compares this BsonTimestamp to another BsonTimestamp.
        /// </summary>
        /// <param name="other">The other BsonTimestamp.</param>
        /// <returns>A 32-bit signed integer that indicates whether this BsonTimestamp is less than, equal to, or greather than the other.</returns>
        public int CompareTo(BsonTimestamp other)
        {
            if (other == null) { return 1; }
            return _value.CompareTo(other._value);
        }

        /// <summary>
        /// Compares the BsonTimestamp to another BsonValue.
        /// </summary>
        /// <param name="other">The other BsonValue.</param>
        /// <returns>A 32-bit signed integer that indicates whether this BsonTimestamp is less than, equal to, or greather than the other BsonValue.</returns>
        public override int CompareTo(BsonValue other)
        {
            if (other == null) { return 1; }
            var otherTimestamp = other as BsonTimestamp;
            if (otherTimestamp != null)
            {
                return _value.CompareTo(otherTimestamp._value);
            }
            var otherDateTime = other as BsonDateTime;
            if (otherDateTime != null)
            {
                var seconds = (int)(otherDateTime.MillisecondsSinceEpoch / 1000);
                var otherTimestampValue = ((long)seconds) << 32;
                return _value.CompareTo(otherTimestampValue);
            }
            return CompareTypeTo(other);
        }

        /// <summary>
        /// Compares this BsonTimestamp to another BsonTimestamp.
        /// </summary>
        /// <param name="rhs">The other BsonTimestamp.</param>
        /// <returns>True if the two BsonTimestamp values are equal.</returns>
        public bool Equals(BsonTimestamp rhs)
        {
            if (object.ReferenceEquals(rhs, null) || GetType() != rhs.GetType()) { return false; }
            return _value == rhs._value;
        }

        /// <summary>
        /// Compares this BsonTimestamp to another object.
        /// </summary>
        /// <param name="obj">The other object.</param>
        /// <returns>True if the other object is a BsonTimestamp and equal to this one.</returns>
        public override bool Equals(object obj)
        {
            return Equals(obj as BsonTimestamp); // works even if obj is null or of a different type
        }

        /// <summary>
        /// Gets the hash code.
        /// </summary>
        /// <returns>The hash code.</returns>
        public override int GetHashCode()
        {
            // see Effective Java by Joshua Bloch
            int hash = 17;
            hash = 37 * hash + BsonType.GetHashCode();
            hash = 37 * hash + _value.GetHashCode();
            return hash;
        }

        /// <summary>
        /// Returns a string representation of the value.
        /// </summary>
        /// <returns>A string representation of the value.</returns>
        public override string ToString()
        {
            return XmlConvert.ToString(_value);
        }
    }
}
