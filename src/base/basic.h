/**********************************************************
 * Classes:
 *     Error                                   :内部错误
 *     NonCopyable                             :不可复制对象
 * Macros:
 *     CHECK_ERROR(CONDITION, DESCRITION)      :检查内部错误
 ************************************************************/

#ifndef ZLPP_BASIC_H_
#define ZLPP_BASIC_H_



namespace zh
{

#define SYNC_ADD(x) __sync_add_and_fetch(x, 1)
#define SYNC_SUB(x) __sync_sub_and_fetch(x, 1)


class Error
{
  private:
    const wchar_t *description;

  public:
    Error(const wchar_t *_description);

    const wchar_t *Description() const;
};

class NonCopyable
{
  private:
    NonCopyable(const NonCopyable &);
    NonCopyable &operator=(const NonCopyable &);

  public:
    NonCopyable();
};
}

#define CHECK_ERROR(CONDITION, DESCRIPTION) \
    do                                      \
    {                                       \
        if (!(CONDITION))                   \
            throw Error(DESCRIPTION);       \
    } while (0)

#endif
