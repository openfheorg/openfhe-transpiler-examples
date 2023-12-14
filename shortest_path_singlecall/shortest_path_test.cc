// Copyright 2022 Duality Technologies Inc.
// D. Cousins
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "vector_compare.h"
#include "vector_max.h"
#include "vector_min.h"

#include "gtest/gtest.h"

TEST(VectorCompareTest, Basic) {
  int x[LEN];

  int y[LEN];
  bool res_cmp[LEN];

  int i;

  for (i=0; i < LEN; i++){
	x[i] = i*10-5;
	y[i] = -1*(i%2)*10;A
	res_cmp[i] = x[i]>y[i];

	cout << "inputs are " << x <<
	" and " << y << ", compare: " << (x > y) << endl;

  }		 
  
  bool result[LEN] = vector_compare(64, 45);
  for (i=0; i < LEN; i++){
	EXPECT_EQ(result[i], res_cmp[i]);
  }
}
#if 0
TEST(VectorCompareTest, NegativeValue) {
  bool result = vector_compare(64, -45);

  EXPECT_EQ(result, true);
}

TEST(VectorCompareTest, TwoNegativeValues) {
  bool result = vector_compare(-64, -45);

  EXPECT_EQ(result, false);
}

TEST(VectorMaxTest, Basic) {
  int result = vector_max(64, 45);

  EXPECT_EQ(result, 64);
}

TEST(VectorMaxTest, NegativeValue) {
  int result = vector_max(64, -45);

  EXPECT_EQ(result, 64);
}

TEST(VectorMaxTest, TwoNegativeValues) {
  int result = vector_max(-64, -45);

  EXPECT_EQ(result, -45);
}

TEST(VectorMinTest, Basic) {
  int result = vector_min(64, 45);

  EXPECT_EQ(result, 45);
}

TEST(VectorMinTest, NegativeValue) {
  int result = vector_min(64, -45);

  EXPECT_EQ(result, -45);
}

TEST(VectorMinTest, TwoNegativeValues) {
  int result = vector_min(-64, -45);

  EXPECT_EQ(result, -64);
}
#endif
