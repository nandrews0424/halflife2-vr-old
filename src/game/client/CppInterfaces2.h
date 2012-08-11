//
// CppInterfaces2.h
//
 
#define Interface class
 
#define implements public
 
#define DeclareInterface(name) __interface actual_##name {
 
#define DeclareBasedInterface(name, base) __interface actual_##name \
     : public actual_##base {
 
#define EndInterface(name) };                \
     Interface name : public actual_##name { \
     public:                                 \
        virtual ~name() {}                   \
     };