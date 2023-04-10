# -*- coding: utf-8 -*-
from src import gzip2header

Import("env")

# compress HTML before building App image.
env.AddPreAction("$BUILD_DIR/app_httpd.cpp", gzip2header.main())
