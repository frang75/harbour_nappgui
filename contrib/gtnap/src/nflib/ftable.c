/* Form tableview */

#include "ftable.h"
#include "nflib.inl"
#include <gui/tableview.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FTable *ftable_create(void)
{
    return dbind_create(FTable);
}

/*---------------------------------------------------------------------------*/

void ftable_destroy(FTable **table)
{
    dbind_destroy(table, FTable);
}

/*---------------------------------------------------------------------------*/

void ftable_synchro(const FTable *table, TableView *view)
{
    uint32_t i = 0, n = tableview_column_count(view);
    cassert_no_null(table);
    for (i = 0; i < n; ++i)
        tableview_del_column(view, 0);

    tableview_size(view, s2df(table->min_width, table->min_height));
    tableview_header_resizable(view, TRUE);
    arrst_foreach_const(header, table->headers, FHeader)
        uint32_t id = tableview_add_column_text(view);
        cassert(id == header_i);
        tableview_column_width(view, id, header->width);
        tableview_column_limits(view, id, header->min_width, header->max_width);
        tableview_column_align(view, id, _nflib_halign(header->dalign));
        tableview_column_resizable(view, id, header->resizable);
        tableview_header_title(view, id, tc(header->title));
        tableview_header_align(view, id, _nflib_halign(header->align));
    arrst_end()
}
