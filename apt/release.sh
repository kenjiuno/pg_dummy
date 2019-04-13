#!/bin/bash

# See: https://unix.stackexchange.com/questions/403485/how-to-generate-the-release-file-on-a-local-package-repository

(
  apt-ftparchive release -c=aptftp.conf dists/$1 >dists/$1/Release
) && (
  rm -f dists/$1/Release.gpg dists/$1/InRelease
) && (
  gpg --clearsign -o dists/$1/InRelease dists/$1/Release
) && (
  gpg -abs -o dists/$1/Release.gpg dists/$1/Release
)
