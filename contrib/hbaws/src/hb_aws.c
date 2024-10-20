/*
 * AWS Harbour Wrapper
 * 2024 - Francisco Garcia Collado
 */

#include "hbaws.h"
#include "hbaws.ch"
#include "hbapiitm.h"
#include "hbset.h"

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_INIT)
{
    HB_ITEM *access_key_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *secret_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_BOOL ret = hb_aws_init(access_key_block, secret_block);

    if (ret)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ret);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_FINISH)
{
    hb_aws_finish();
}

/*---------------------------------------------------------------------------*/

static void i_object_list(const S3Objs *objects, int error_pos)
{
    PHB_ITEM ret = NULL;
    if (objects != NULL)
    {
        int size = hb_aws_s3_size(objects);
        ret = hb_itemArrayNew(size);
        for (int i = 0; i < size; ++i)
        {
            PHB_ITEM item = hb_itemArrayNew(13);
            hb_arraySetC(item, OBJ_S3KEY, hb_aws_s3_key(objects, i));
            hb_arraySetNL(item, OBJ_CONTENT_SIZE, (long)hb_aws_s3_content_size(objects, i));
            hb_arraySetC(item, OBJ_CONTENT_TYPE, hb_aws_s3_content_type(objects, i));
            hb_arraySetDS(item, OBJ_DATE, hb_aws_s3_date(objects, i));
            hb_arraySetC(item, OBJ_TIME, hb_aws_s3_time(objects, i));
            hb_arraySetC(item, OBJ_TIMEZONE, hb_aws_s3_timezone(objects, i));
            hb_arraySetC(item, OBJ_STORAGE_CLASS, hb_aws_s3_storage_class(objects, i));
            hb_arraySetL(item, OBJ_IS_RESTORE, hb_aws_s3_is_restore(objects, i));
            hb_arraySetDS(item, OBJ_RESTORE_DATE, hb_aws_s3_restore_date(objects, i));
            hb_arraySetC(item, OBJ_RESTORE_TIME, hb_aws_s3_restore_time(objects, i));
            hb_arraySetC(item, OBJ_RESTORE_TIMEZONE, hb_aws_s3_restore_timezone(objects, i));
            hb_arraySetC(item, OBJ_CHECKSUM_ALGORITHM, hb_aws_s3_checksum_algorithm(objects, i));
            hb_arraySetC(item, OBJ_ETAG, hb_aws_s3_etag(objects, i));
            hb_arraySet(ret, i + 1, item);
        }

        // No error
        hb_storc("", error_pos);
    }
    else
    {
        ret = hb_itemArrayNew(0);
        hb_storc(hb_aws_last_error(), error_pos);
    }

    hb_itemReturnRelease(ret);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_LIST_ALL)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *prefix_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    const S3Objs *objects = hb_aws_s3_list_all(bucket_block, prefix_block);
    i_object_list(objects, 1);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_LIST_PAGINATED)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *prefix_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *start_after_block = hb_param(4, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    int max_keys = hb_parni(5);
    HB_ITEM *continuation_token_block = hb_param(6, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    const char *next_continuation_token = NULL;
    const S3Objs *objects = hb_aws_s3_list_page(bucket_block, prefix_block, start_after_block, continuation_token_block, max_keys, &next_continuation_token);

    if (objects == NULL || strcmp(next_continuation_token, "") == 0)
        hb_stor(7); // NIL
    else
        hb_storc(next_continuation_token, 7);

    i_object_list(objects, 1);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_UPLOAD_SIMPLE)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *local_file_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *remote_key_block = hb_param(4, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *content_type_block = hb_param(5, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    s3_storage_class_t storage = (s3_storage_class_t)hb_parni(6);
    HB_BOOL ok = hb_aws_s3_upload_simple(bucket_block, local_file_block, remote_key_block, content_type_block, storage);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_UPLOAD_MULTIPART)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *local_file_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *remote_key_block = hb_param(4, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *content_type_block = hb_param(5, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    s3_storage_class_t storage = (s3_storage_class_t)hb_parni(6);
    uint32_t chunk_size = hb_parni(7);
    uint32_t num_retries = hb_parni(8);
    HB_BOOL ok = hb_aws_s3_upload_multipart(bucket_block, local_file_block, remote_key_block, content_type_block, storage, chunk_size, num_retries);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_COPY_SIMPLE)
{
    HB_ITEM *src_bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *src_key_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *dest_bucket_block = hb_param(4, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *dest_key_block = hb_param(5, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *dest_content_type_block = hb_param(6, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    s3_storage_class_t dest_storage = (s3_storage_class_t)hb_parni(7);
    HB_BOOL ok = hb_aws_s3_copy_simple(src_bucket_block, src_key_block, dest_bucket_block, dest_key_block, dest_content_type_block, dest_storage);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_COPY_MULTIPART)
{
    HB_ITEM *src_bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *src_key_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *dest_bucket_block = hb_param(4, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *dest_key_block = hb_param(5, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *dest_content_type_block = hb_param(6, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    s3_storage_class_t dest_storage = (s3_storage_class_t)hb_parni(7);
    uint32_t chunk_size = hb_parni(8);
    uint32_t num_retries = hb_parni(9);
    HB_BOOL ok = hb_aws_s3_copy_multipart(src_bucket_block, src_key_block, dest_bucket_block, dest_key_block, dest_content_type_block, dest_storage, chunk_size, num_retries);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_DOWNLOAD)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *key_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *local_file_block = hb_param(4, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_BOOL ok = hb_aws_s3_download(bucket_block, key_block, local_file_block);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_DELETE)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *key_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_BOOL ok = hb_aws_s3_delete(bucket_block, key_block);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBAWS_S3_RESTORE)
{
    HB_ITEM *bucket_block = hb_param(2, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    HB_ITEM *key_block = hb_param(3, HB_IT_STRING | HB_IT_BLOCK | HB_IT_NIL);
    int num_days = hb_parni(4);
    s3_tier_t tier = (s3_tier_t)hb_parni(5);
    HB_BOOL ok = hb_aws_s3_restore(bucket_block, key_block, num_days, tier);

    if (ok)
        hb_storc("", 1);
    else
        hb_storc(hb_aws_last_error(), 1);

    hb_retl(ok);
}
