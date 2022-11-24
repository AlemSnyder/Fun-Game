#!/usr/bin/env bash
# -*- coding: utf-8 -*-

shopt -s extglob                   # For making ** apply to any number of dirs

clang-format src/**/*.{h,c}pp -i
