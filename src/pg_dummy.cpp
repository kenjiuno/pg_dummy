#include <postgres.h>
#include <stdint.h>

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(dummy_normalize);

int dummy_normalizeImpl(
    const char *textIn,
    int bytesTextIn,
    char *textOut,
    int maximumBytesTextOut)
{
    for (int x = 0; x < bytesTextIn; x++)
    {
        char one = textIn[x];
        if ('A' <= one && one <= 'Z')
        {
            one = one - 'A' + 'a';
        }
        textOut[x] = one;
    }
    return bytesTextIn;
}

Datum dummy_normalize(PG_FUNCTION_ARGS)
{
    if (PG_NARGS() != 1 || PG_ARGISNULL(0))
    {
        PG_RETURN_NULL();
    }

    {
        text *textIn = PG_GETARG_TEXT_P(0);
        size_t bytesTextIn = VARSIZE(textIn);
        size_t bytesTextOut = VARSIZE(textIn);
        text *textOut = (text *)palloc(bytesTextOut);

        int outLen = dummy_normalizeImpl(
            VARDATA(textIn), bytesTextIn - VARHDRSZ,
            VARDATA(textOut), bytesTextOut - VARHDRSZ);

        SET_VARSIZE(textOut, VARHDRSZ + outLen);

        PG_RETURN_TEXT_P(textOut);
    }
}
