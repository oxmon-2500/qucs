/*
 * test_trsolver.cpp - Miscellaneous unit tests for Qucs core library
 *
 * Copyright (C) 2014, 2015 Guilherme Brondani Torri <guitorri@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <iostream>

#include "qucs_typedefs.h"
#include "object.h"
#include "trsolver.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test

using namespace qucs;
TEST (trsolver, constructor) {
  //std::cout << "can we print info"  << std::endl;
  trsolver *trs = new trsolver("szb"); //named instance
  trs->clearStates();
  //EXPECT_EQ(NULL, m->definition);
}
