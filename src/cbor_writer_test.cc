// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "dice/cbor_writer.h"

#include "pw_unit_test/framework.h"

namespace {

extern "C" {

TEST(CborWriterTest, Int1ByteEncoding) {
  const uint8_t kExpectedEncoding[] = {0, 23, 0x20, 0x37};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(1u, CborWriteInt(0, &out));
  EXPECT_EQ(1u, CborWriteInt(23, &out));
  EXPECT_EQ(1u, CborWriteInt(-1, &out));
  EXPECT_EQ(1u, CborWriteInt(-24, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, Int2Bytes) {
  const uint8_t kExpectedEncoding[] = {24, 24, 24, 0xff, 0x38, 24, 0x38, 0xff};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(2u, CborWriteInt(24, &out));
  EXPECT_EQ(2u, CborWriteInt(0xff, &out));
  EXPECT_EQ(2u, CborWriteInt(-25, &out));
  EXPECT_EQ(2u, CborWriteInt(-0x100, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, Int3Bytes) {
  const uint8_t kExpectedEncoding[] = {25,   0x01, 0x00, 25,   0xff, 0xff,
                                       0x39, 0x01, 0x00, 0x39, 0xff, 0xff};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(3u, CborWriteInt(0x100, &out));
  EXPECT_EQ(3u, CborWriteInt(0xffff, &out));
  EXPECT_EQ(3u, CborWriteInt(-0x101, &out));
  EXPECT_EQ(3u, CborWriteInt(-0x10000, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, Int5Bytes) {
  const uint8_t kExpectedEncoding[] = {26,   0x00, 0x01, 0x00, 0x00, 26,   0xff,
                                       0xff, 0xff, 0xff, 0x3a, 0x00, 0x01, 0x00,
                                       0x00, 0x3a, 0xff, 0xff, 0xff, 0xff};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(5u, CborWriteInt(0x10000, &out));
  EXPECT_EQ(5u, CborWriteInt(0xffffffff, &out));
  EXPECT_EQ(5u, CborWriteInt(-0x10001, &out));
  EXPECT_EQ(5u, CborWriteInt(-0x100000000, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, Int9Bytes) {
  const uint8_t kExpectedEncoding[] = {
      27,   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 27,   0x7f, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3b, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(9u, CborWriteInt(0x100000000, &out));
  EXPECT_EQ(9u, CborWriteInt(INT64_MAX, &out));
  EXPECT_EQ(9u, CborWriteInt(-0x100000001, &out));
  EXPECT_EQ(9u, CborWriteInt(INT64_MIN, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, IntByteOrder) {
  const uint8_t kExpectedEncoding[] = {
      25,   0x12, 0x34, 26,   0x12, 0x34, 0x56, 0x78, 27,
      0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0,
  };
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(3u, CborWriteInt(0x1234, &out));
  EXPECT_EQ(5u, CborWriteInt(0x12345678, &out));
  EXPECT_EQ(9u, CborWriteInt(0x123456789abcdef0, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, BstrEncoding) {
  const uint8_t kExpectedEncoding[] = {0x45, 'h', 'e', 'l', 'l', 'o'};
  const uint8_t kData[] = {'h', 'e', 'l', 'l', 'o'};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding),
            CborWriteBstr(sizeof(kData), kData, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, TstrEncoding) {
  const uint8_t kExpectedEncoding[] = {0x65, 'w', 'o', 'r', 'l', 'd'};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding), CborWriteTstr("world", &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, ArrayEncoding) {
  const uint8_t kExpectedEncoding[] = {0x98, 29};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding),
            CborWriteArray(/*num_elements=*/29, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, MapEncoding) {
  const uint8_t kExpectedEncoding[] = {0xb9, 0x02, 0x50};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding), CborWriteMap(/*num_pairs=*/592, &out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, FalseEncoding) {
  const uint8_t kExpectedEncoding[] = {0xf4};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding), CborWriteFalse(&out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, TrueEncoding) {
  const uint8_t kExpectedEncoding[] = {0xf5};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding), CborWriteTrue(&out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, NullEncoding) {
  const uint8_t kExpectedEncoding[] = {0xf6};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(sizeof(kExpectedEncoding), CborWriteNull(&out));
  EXPECT_EQ(0, memcmp(buffer, kExpectedEncoding, sizeof(kExpectedEncoding)));
}

TEST(CborWriterTest, CborOutInvariants) {
  const uint8_t kData[] = {0xb2, 0x34, 0x75, 0x92, 0x52};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  EXPECT_EQ(3u, CborWriteInt(0xab34, &out));
  EXPECT_EQ(6u, CborWriteBstr(sizeof(kData), kData, &out));
  EXPECT_EQ(9u, CborWriteTstr("A string", &out));
  EXPECT_EQ(1u, CborWriteArray(/*num_elements=*/16, &out));
  EXPECT_EQ(2u, CborWriteMap(/*num_pairs=*/35, &out));
  EXPECT_EQ(1u, CborWriteFalse(&out));
  EXPECT_EQ(1u, CborWriteTrue(&out));
  EXPECT_EQ(1u, CborWriteNull(&out));
  // Offset is the cumulative size.
  EXPECT_EQ(3 + 6 + 9 + 1 + 2 + 1 + 1 + 1u, out.offset);
  // Buffer and size are unchanged.
  EXPECT_EQ(buffer, out.buffer);
  EXPECT_EQ(sizeof(buffer), out.size);
}

TEST(CborWriterTest, NullBufferForMeasurement) {
  const uint8_t kData[] = {16, 102, 246, 12, 156, 35, 84};
  CborOut out = {
      .buffer = nullptr,
      .size = 2,  // Ignored.
  };
  EXPECT_EQ(1u, CborWriteNull(&out));
  EXPECT_EQ(1u, CborWriteTrue(&out));
  EXPECT_EQ(1u, CborWriteFalse(&out));
  EXPECT_EQ(3u, CborWriteMap(/*num_pairs=*/623, &out));
  EXPECT_EQ(5u, CborWriteArray(/*num_elements=*/70000, &out));
  EXPECT_EQ(7u, CborWriteTstr("length", &out));
  EXPECT_EQ(8u, CborWriteBstr(sizeof(kData), kData, &out));
  EXPECT_EQ(5u, CborWriteInt(-10002000, &out));
  // Offset is the cumulative size.
  EXPECT_EQ(1 + 1 + 1 + 3 + 5 + 7 + 8 + 5u, out.offset);
  // Buffer and size are unchanged.
  EXPECT_EQ(nullptr, out.buffer);
  EXPECT_EQ(2u, out.size);
}

TEST(CborWriterTest, BufferTooSmall) {
  const uint8_t kData[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  uint8_t buffer[1];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  // Reset offset each time as it may be corrupted on failures.
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteInt(-55667788, &out));
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteBstr(sizeof(kData), kData, &out));
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteTstr("Buffer too small", &out));
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteArray(/*num_elements=*/563, &out));
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteMap(/*num_pairs=*/29, &out));
  out.size = 0;
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteFalse(&out));
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteTrue(&out));
  out.offset = 0;
  EXPECT_EQ(0u, CborWriteNull(&out));
}

TEST(CborWriterTest, NotEnoughRemainingSpace) {
  const uint8_t kData[] = {0xff, 0xee, 0xdd, 0xcc};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  // Reset offset each time as it may be corrupted on failures.
  out.offset = sizeof(buffer) - 1;
  EXPECT_EQ(0u, CborWriteInt(-36, &out));
  out.offset = sizeof(buffer) - 1;
  EXPECT_EQ(0u, CborWriteBstr(sizeof(kData), kData, &out));
  out.offset = sizeof(buffer) - 1;
  EXPECT_EQ(0u, CborWriteTstr("Won't fit", &out));
  out.offset = sizeof(buffer) - 1;
  EXPECT_EQ(0u, CborWriteArray(/*num_elements=*/352, &out));
  out.offset = sizeof(buffer) - 1;
  EXPECT_EQ(0u, CborWriteMap(/*num_pairs=*/73, &out));
  out.offset = sizeof(buffer);
  EXPECT_EQ(0u, CborWriteFalse(&out));
  out.offset = sizeof(buffer);
  EXPECT_EQ(0u, CborWriteTrue(&out));
  out.offset = sizeof(buffer);
  EXPECT_EQ(0u, CborWriteNull(&out));
}

TEST(CborWriterTest, OffsetOverflow) {
  const uint8_t kData[] = {0xff, 0xee, 0xdd, 0xcc};
  uint8_t buffer[64];
  CborOut out = {
      .buffer = buffer,
      .size = sizeof(buffer),
  };
  // Reset offset each time as it may be corrupted on failures.
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteInt(0x234198adb, &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteBstr(sizeof(kData), kData, &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteTstr("Overflow", &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteArray(/*num_elements=*/41, &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteMap(/*num_pairs=*/998844, &out));
  out.offset = SIZE_MAX;
  EXPECT_EQ(0u, CborWriteFalse(&out));
  out.offset = SIZE_MAX;
  EXPECT_EQ(0u, CborWriteTrue(&out));
  out.offset = SIZE_MAX;
  EXPECT_EQ(0u, CborWriteNull(&out));
}

TEST(CborWriterTest, MeasurementOffsetOverflow) {
  const uint8_t kData[] = {0xf0, 0x0f, 0xca, 0xfe, 0xfe, 0xed};
  CborOut out = {
      .buffer = nullptr,
  };
  // Reset offset each time as it may be corrupted on failures.
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteInt(0x1419823646241245, &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteBstr(sizeof(kData), kData, &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteTstr("Measured overflow", &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteArray(/*num_elements=*/8368257314, &out));
  out.offset = SIZE_MAX - 1;
  EXPECT_EQ(0u, CborWriteMap(/*num_pairs=*/92, &out));
  out.offset = SIZE_MAX;
  EXPECT_EQ(0u, CborWriteFalse(&out));
  out.offset = SIZE_MAX;
  EXPECT_EQ(0u, CborWriteTrue(&out));
  out.offset = SIZE_MAX;
  EXPECT_EQ(0u, CborWriteNull(&out));
}
}

}  // namespace