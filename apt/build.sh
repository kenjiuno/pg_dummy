#!/bin/bash

(
  find . -name "Contents-amd64.*" -or -name "Packages" -or -name "Packages.*" -delete
) && (
  apt-ftparchive generate -c=aptftp.conf aptgenerate.conf
) && (
  ./release.sh myrepo
) && (
  echo OK
)
