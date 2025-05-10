#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import cgi
import os
import sys

# 업로드될 파일 저장 경로 (절대경로로 설정)
UPLOAD_DIR = "/Users/woongbishin/Woongbi_File/42seoul/NotNginx/html/upload"

form = cgi.FieldStorage()

if "file" not in form:
    print "No file uploaded"
    sys.exit(0)

file_item = form["file"]

# 파일이 업로드된 경우
if file_item.filename:
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)

    # 파일 이름 추출 및 저장 경로 생성
    filename = os.path.basename(file_item.filename)
    filepath = os.path.join(UPLOAD_DIR, filename)

    with open(filepath, "wb") as f:
        f.write(file_item.file.read())

    print "File '{}' uploaded successfully to {}".format(filename, UPLOAD_DIR)
else:
    print "File field is empty"
