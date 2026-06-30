struct _objc_typeinfo { void *vtable[2]; const char *name; };
static struct _objc_typeinfo _ehtype_id_data = {{0, 0}, "id"};
static struct _objc_typeinfo _ehtype_nsexception_data = {{0, 0}, "NSException"};
struct _objc_typeinfo *OBJC_EHTYPE_id = &_ehtype_id_data;
struct _objc_typeinfo *OBJC_EHTYPE_$_NSException = &_ehtype_nsexception_data;
void __objc_personality_v0(void) {}
