# Harbour AWS wrapper (hbaws)

* [Introduction](#introduction)
* [Build HBAWS in Windows](#build-hbaws-in-windows)
* [Build HBAWS in Linux](#build-hbaws-in-linux)
* [Build HBAWS in macOS](#build-hbaws-in-macos)
* [HBAWS examples](#hbaws-examples)
* [Reference guide](#reference-guide)
    - [HBAWS_INIT](#hbaws_init)
    - [HBAWS_FINISH](#hbaws_finish)
    - [HBAWS_S3_LIST_ALL](#hbaws_s3_list_all)
    - [HBAWS_S3_LIST_PAGINATED](#hbaws_s3_list_paginated)
    - [HBAWS_S3_UPLOAD_SIMPLE](#hbaws_s3_upload_simple)
    - [HBAWS_S3_UPLOAD_MULTIPART](#hbaws_s3_upload_multipart)
    - [HBAWS_S3_COPY_SIMPLE](#hbaws_s3_copy_simple)
    - [HBAWS_S3_COPY_MULTIPART](#hbaws_s3_copy_multipart)
    - [HBAWS_S3_DOWNLOAD](#hbaws_s3_download)
    - [HBAWS_S3_DELETE](#hbaws_s3_delete)
    - [HBAWS_S3_RESTORE](#hbaws_s3_restore)
* [For hbaws developers](#for-hbaws-developers)

## Introduction

**hbaws** is a project that allows us to connect to Amazon Web Services (AWS) directly from Harbour. At the moment, only some features of the S3 service are accessible, but this may be expanded in the future. It provides high-level functions in C that hide the complexity of HTTP requests. This C API is easily portable to Harbour.

**hbaws** communicates with Amazon servers through the AWS-SDK-CPP, a project maintained by Amazon that encapsulates the complexity of making calls using the REST-API. The required AWS-SDK-CPP headers and binaries are vendored directly in the `awssdk` folder, platform by platform, so end users don't need to download/build AWS-SDK-CPP themselves once their platform is vendored.

> **Important:** The AWSSDK binaries corresponding to each build (`awssdk/bin/win/msvc`, `awssdk/bin/win/gcc`, `awssdk/bin/linux/gcc` or `awssdk/bin/darwin/clang`) must be distributed with the final application for it to work.

> **Important:** In Windows, MSVC binaries (`awssdk/bin/win/msvc`) and mingw64/clang binaries (`awssdk/bin/win/gcc`) are **not** ABI-compatible with each other (different C++ name mangling/runtime), therefore they are not interchangeable.


## Build HBAWS in Windows

To build the `hbaws` library, just run the `build.bat` script. We can use `mingw64`, `clang` or `msvc64`.

```
cd contrib\hbaws
build -b [Debug|Release] -comp mingw64
build -b [Debug|Release] -comp clang

:: Set Visual Studio 2026 64bit compiler for hbmk2 (msvc)
"%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
build -b [Debug|Release] -comp msvc64
```

The `libhbaws.a` (mingw64, clang) or `hbaws.lib` (msvc64) will be generated in `hbaws\build\[Debug|Release]\lib`.

## Build HBAWS in Linux

```
cd contrib/hbaws
build -comp gcc -b [Debug|Release]
build -comp clang -b [Debug|Release]
```

The `libhbaws.a` will be generated in `hbaws/build/[Debug|Release]/lib`.

## Build HBAWS in macOS

```
cd contrib/hbaws
export MACOSX_DEPLOYMENT_TARGET=13.0  # Ventura
./build.sh -comp clang -b [Debug|Release]
```

The `libhbaws.a` will be generated in `hbaws/build/[Debug|Release]/lib`.

## HBAWS examples

Some examples have been provided in `contrib/hbaws/tests/harbour`.

> **Important:** Before running examples, open the `credentials.prg` and fill the required data.

On **Windows**, `tests/harbour/test.bat` builds all the examples and points `PATH` to the vendored AWS-SDK DLLs, for any of the three supported compilers.

```
cd contrib\hbaws\tests\harbour
test -comp mingw64
test -comp clang

:: Set Visual Studio 2026 64bit compiler for hbmk2 (msvc)
"%ProgramFiles%\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
test -comp msvc64
```

On **Linux/macOS** `tests/harbour/test.sh` will perform the compilation, setting `LD_LIBRARY_PATH` or `DYLD_LIBRARY_PATH`. Run it with `source` (or `.`) if you want that variable to stay set in your shell afterwards, to run the resulting executables right away:

```
cd contrib/hbaws/tests/harbour
source test.sh -comp gcc
source test.sh -comp clang
```

* `listall`: Use of `HBAWS_S3_LIST_ALL` function.
* `listpage`: Use of `HBAWS_S3_LIST_PAGINATED` function.
* `upload`: Use of `HBAWS_S3_UPLOAD_SIMPLE` function.
* `uploadm`: Use of `HBAWS_S3_UPLOAD_MULTIPART` function.
* `copy`: Use of `HBAWS_S3_COPY_SIMPLE` function.
* `copym`: Use of `HBAWS_S3_COPY_MULTIPART` function.
* `download`: Use of `HBAWS_S3_DOWNLOAD` function.
* `delete`: Use of `HBAWS_S3_DELETE` function.
* `restore`: Use of `HBAWS_S3_RESTORE` function.

## Reference guide

### HBAWS_INIT

This function must be call before any other. It remember the session. Only one call is required.

```
LOCAL L_OK := HBAWS_INIT(@C_ERR, C_AccessKey, C_Secret)

PAR1: Reference string to store the error message (if any).
PAR2: Access key.
PAR3: Secret.
RET: .T. if connection with AWS can be performed. If .F., in C_ERR will be stored the error.
```

### HBAWS_FINISH

This function must be called before the program terminates or calls to AWS are no longer needed.

```
HBAWS_FINISH()

No parameters neither return value.
```

### HBAWS_S3_LIST_ALL

Return a list of ALL files in the bucket, breaking the AWS 1000 items limit.

```
LOCAL V_OBJS := HBAWS_S3_LIST_ALL(@C_ERR, C_BUCKET, C_PREFIX)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket where the list will be performed.
PAR3: Filter prefix (optional, allow NIL or empty string).
RET: A vector of vectors with all objects. { {Obj1}, {Obj2}, ..., {ObjN} }.

* If error, will return an empty vector {} and C_ERR will contain the error message.

* Every inner {Obj} vector has this info:
  - S3Key: Obj[OBJ_S3KEY]
  - ContentSize: hb_ntos(Obj[OBJ_CONTENT_SIZE])
  - ContentType: Obj[OBJ_CONTENT_TYPE]
  - Date: DToC(Obj[OBJ_DATE])
  - Time: Obj[OBJ_TIME]
  - TimeZone: Obj[OBJ_TIMEZONE]
  - StorageClass: Obj[OBJ_STORAGE_CLASS]
  - IsRestore: hb_ValToStr(Obj[OBJ_IS_RESTORE])
  - RestoreDate: DToC(Obj[OBJ_RESTORE_DATE])
  - RestoreTime: Obj[OBJ_RESTORE_TIME]
  - RestoreTimeZone: Obj[OBJ_RESTORE_TIMEZONE]
  - ChecksumAlgorithm: Obj[OBJ_CHECKSUM_ALGORITHM]
  - ETag: Obj[OBJ_ETAG]
```

### HBAWS_S3_LIST_PAGINATED

Return a single page of the list files in the bucket, returning a continuation token for a possible next call.

```
LOCAL V_OBJS := HBAWS_S3_LIST_PAGINATED(@C_ERR, C_BUCKET, C_PREFIX, C_START_AFTER, N_MAX_KEYS, C_CONTINUATION_TOKEN, @C_NEXT_CONTINUATION_TOKEN)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket where the list will be performed.
PAR3: Filter prefix (optional, allow NIL or empty string).
PAR4: Object key for begin the search (optional, allow NIL or empty string).
PAR5: Maximum number of objects returned. If >1000, the page will be limited to 1000.
PAR6: Previous continuation token. If NIL of empty the search will start by the beginning.
PAR6: Reference string to store the next continuation token for possible next call. If no more pages, NIL will be stored.
RET: A vector of vectors with all objects in the page. { {Obj1}, {Obj2}, ..., {ObjN} }.

* If error, will return an empty vector {} and C_ERR will contain the error message.
* Every inner {Obj} vector has the same info as HBAWS_S3_LIST_ALL.
```

### HBAWS_S3_UPLOAD_SIMPLE

Upload a file to AWS-S3 bucket in a single request. Use this function for relative small sized files.

```
LOCAL L_OK := HBAWS_S3_UPLOAD_SIMPLE(@C_ERR, C_BUCKET, C_LOCAL_FILE, C_KEY, C_CONTENT_TYPE, N_STORAGE)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket name to upload.
PAR3: The local path with the file to be uploaded.
PAR4: The key of the uploaded file in AWS-S3.
PAR5: The mime content-type. E.g: "image/svg+xml"
PAR6: Storage class numeric value.
RET: .T. if upload is success or .F. if error.

The Storage class values are defined in 'hbaws.ch'
#define STORAGE_STANDARD 1
#define STORAGE_REDUCED_REDUNDANCY 2
#define STORAGE_STANDARD_IA 3
#define STORAGE_ONEZONE_IA 4
#define STORAGE_INTELLIGENT_TIERING 5
#define STORAGE_GLACIER 6
#define STORAGE_DEEP_ARCHIVE 7
#define STORAGE_OUTPOSTS 8
#define STORAGE_GLACIER_IR 9
#define STORAGE_SNOW 10
#define STORAGE_EXPRESS_ONEZONE 11
```

### HBAWS_S3_UPLOAD_MULTIPART

Upload a file to AWS-S3 bucket using a multipart request model. Use this function for really big files.

```
LOCAL L_OK := HBAWS_S3_UPLOAD_MULTIPART(@C_ERR, C_BUCKET, C_LOCAL_FILE, C_KEY, C_CONTENT_TYPE, N_STORAGE, N_CHUNK_SIZE, N_REQUESTS)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket name to upload.
PAR3: The local path with the file to be uploaded.
PAR4: The key of the uploaded file in AWS-S3.
PAR5: The mime content-type. E.g: "image/svg+xml"
PAR6: Storage class numeric value.
PAR7: Amount of BYTES to upload in each single request. AWS establish, at least, 5Mb (5 * 1024 * 1024). If this parameter is smaller, will be omitted and 5Mb will be set.
PAR8: Number of retries if any single part upload fails.
RET: .T. if upload is success or .F. if error.

IMPORTANT: If upload fail, 'AbortMultipartUpload()' will be called to clean any part of file in AWS-S3 caches.
```

### HBAWS_S3_COPY_SIMPLE

Copy a file from a AWS-S3 bucket to another bucket. Use user registered in `HBAWS_INIT()` should have read permissions for `source` file and write permissions for `destiny` bucket.

```
LOCAL L_OK := HBAWS_S3_COPY_SIMPLE(@C_ERR, C_SRC_BUCKET, C_SRC_KEY, C_DEST_BUCKET, C_DEST_KEY, C_DEST_CONTENT_TYPE, N_DEST_STORAGE)

PAR1: Reference string to store the error message (if any).
PAR2: Source bucket name.
PAR3: Source key.
PAR4: Destiny bucket name.
PAR5: Destiny key.
PAR6: Content type for destiny file.
PAR7: Storage class for destiny file.
RET: .T. if copy is success or .F. if error.
```

### HBAWS_S3_COPY_MULTIPART

Copy a file from a AWS-S3 bucket to another bucket using a multipart request model. Use user registered in `HBAWS_INIT()` should have read permissions for `source` file and write permissions for `destiny` bucket.

```
LOCAL L_OK := HBAWS_S3_COPY_MULTIPART(@C_ERR, C_SRC_BUCKET, C_SRC_KEY, C_DEST_BUCKET, C_DEST_KEY, C_DEST_CONTENT_TYPE, N_DEST_STORAGE, N_CHUNK_SIZE, N_REQUESTS)

PAR1: Reference string to store the error message (if any).
PAR2: Source bucket name.
PAR3: Source key.
PAR4: Destiny bucket name.
PAR5: Destiny key.
PAR6: Content type for destiny file.
PAR7: Storage class for destiny file.
PAR8: Amount of BYTES to upload in each single request. AWS establish, at least, 5Mb (5 * 1024 * 1024). If this parameter is smaller, will be omitted and 5Mb will be set.
PAR9: Number of retries if any single part upload fails.
RET: .T. if upload is success or .F. if error.

IMPORTANT: If copy fail, 'AbortMultipartUpload()' will be called to clean any part of file in AWS-S3 caches.
```

### HBAWS_S3_DOWNLOAD

Download a file from a AWS-S3 bucket.

```
LOCAL L_OK := HBAWS_S3_DOWNLOAD(@C_ERR, C_BUCKET, C_KEY, C_LOCAL_FILE)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket to download from.
PAR3: The key of the file to download.
PAR4: The local path where the file will be saved.
RET: .T. if download is success or .F. if error.
```

### HBAWS_S3_DELETE

Delete a file from a AWS-S3 bucket.

```
LOCAL L_OK := HBAWS_S3_DELETE(@C_ERR, C_BUCKET, C_KEY)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket to delete from.
PAR3: The key of the file to delete.
RET: .T. if delete is success or .F. if error.
```

IMPORTANT: If the file doesn't exists in the bucket, will return .T.

### HBAWS_S3_RESTORE

Restore a file from a AWS-S3 bucket.

```
LOCAL L_OK := HBAWS_S3_RESTORE(@C_ERR, C_BUCKET, C_KEY, N_NUM_DAYS, N_TIER)

PAR1: Reference string to store the error message (if any).
PAR2: Bucket to restore from.
PAR3: The key of the file to restore.
PAR4: Number of days to restore.
PAR5: Tier.
RET: .T. if restore is success or .F. if error.
```

The Tier values are defined in 'hbaws.ch'
#define TIER_STANDARD 1
#define TIER_BULK 2
#define TIER_EXPEDITED 3

## For hbaws developers

End users of `hbaws` never need to build the AWS-SDK-CPP, it is already vendored in `awssdk/include` and `awssdk/bin/**/{msvc,gcc,clang}`. The `contrib/hbaws/prj` folder holds the tooling only needed to *regenerate* that vendored SDK (e.g. to bump the AWS-SDK-CPP version, or to add a platform that isn't vendored yet):

* `prj/Readme.md`: the previous, full version of this document, with the original step-by-step instructions to download and self-build AWS-SDK-CPP with every supported compiler (MinGW, Clang, MSVC, GCC Linux, Clang macOS).
* `prj/awssdk.bat` / `prj/awssdk.sh`: the scripts that document/perform that self-build.
* `prj/awssdk.patch`: header-only patch required on top of a freshly self-built AWS-SDK-CPP for it to compile under GCC-family compilers (mingw64, Linux gcc). Apply with `git apply contrib/hbaws/prj/awssdk.patch` from the repo root; see `prj/Readme.md` for background.
