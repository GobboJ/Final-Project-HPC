# Define the list of compiler options
add_compile_options(
    # Enabled flags
    -Wall
    -Wextra
    -Wpedantic
    -Waligned-new=all
    -Walloc-zero
    -Walloca
    -Wanalyzer-too-complex
    -Warith-conversion
    -Warray-bounds=2
    -Warray-parameter=2
    -Wattribute-alias=2
    -Wc++0x-compat
    -Wc++11-compat
    -Wc++14-compat
    -Wc++17-compat
    -Wc++1z-compat
    -Wc++20-compat
    -Wc++2a-compat
    -Wcast-align=strict
    -Wcast-qual
    -Wcatch-value=3
    -Wclass-conversion
    -Wclass-memaccess
    -Wcomma-subscript
    -Wconditionally-supported
    -Wconversion
    -Wconversion-null
    -Wctad-maybe-unsupported
    -Wctor-dtor-privacy
    -Wdate-time
    -Wdelete-incomplete
    -Wdelete-non-virtual-dtor
    -Wdeprecated-copy
    -Wdeprecated-copy-dtor
    -Wdeprecated-enum-enum-conversion
    -Wdeprecated-enum-float-conversion
    -Wdisabled-optimization
    -Wdouble-promotion
    -Wduplicated-branches
    -Wduplicated-cond
    -Wexceptions
    -Wextra-semi
    -Wfloat-conversion
    -Wfloat-equal
    -Wformat-nonliteral
    -Wformat-overflow=2
    -Wformat-security
    -Wformat-signedness
    -Wformat-truncation=2
    -Wformat-y2k
    -Wformat=2
    -Wimplicit-fallthrough=5
    -Winaccessible-base
    -Winherited-variadic-ctor
    -Winit-list-lifetime
    -Winit-self
    -Winline
    -Winvalid-imported-macros
    -Winvalid-offsetof
    -Winvalid-pch
    -Wliteral-suffix
    -Wlogical-op
    -Wmismatched-new-delete
    -Wmismatched-tags
    -Wmissing-declarations
    -Wmissing-format-attribute
    -Wmissing-include-dirs
    -Wmissing-noreturn
    -Wmultichar
    -Wmultiple-inheritance
    -Wmultistatement-macros
    -Wnarrowing
    -Wnoexcept
    -Wnoexcept-type
    -Wnon-template-friend
    -Wnon-virtual-dtor
    -Wnormalized=nfkc
    -Wnull-dereference
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpacked
    -Wpacked-bitfield-compat
    -Wpessimizing-move
    -Wplacement-new=2
    -Wpmf-conversions
    -Wrange-loop-construct
    -Wredundant-decls
    -Wredundant-move
    -Wredundant-tags
    -Wregister
    -Wreorder
    -Wno-shadow
    -Wshadow-compatible-local
    -Wshadow-local
    -Wshift-negative-value
    -Wshift-overflow=2
    -Wsign-conversion
    -Wsign-promo
    -Wsized-deallocation
    -Wstrict-aliasing=3
    -Wstrict-null-sentinel
    -Wstrict-overflow=5
    -Wstringop-overflow=4
    -Wsubobject-linkage
    -Wsuggest-attribute=cold
    -Wsuggest-attribute=const
    -Wsuggest-attribute=format
    -Wsuggest-attribute=malloc
    -Wsuggest-attribute=noreturn
    -Wsuggest-attribute=pure
    -Wsuggest-final-methods
    -Wsuggest-final-types
    -Wsuggest-override
    -Wswitch-default
    -Wswitch-enum
    -Wsynth
    -Wtautological-compare
    -Wterminate
    -Wterminate
    -Wtrampolines
    -Wundef
    -Wuninitialized
    -Wunknown-pragmas
    -Wunused-const-variable=2
    -Wunused-macros
    -Wuseless-cast
    -Wvector-operation-performance
    -Wvexing-parse
    -Wvirtual-inheritance
    -Wvirtual-move-assign
    -Wvla
    -Wvolatile
    -Wwrite-strings
    -Wzero-as-null-pointer-constant
    # Disabled flags
    -Wno-aggregate-return
    -Wno-effc++
    -Wno-long-long
    -Wno-missing-field-initializers
    -Wno-namespaces
    -Wno-padded
    -Wno-system-headers
    -Wno-templates
    -Wno-unused-parameter
    # Additional flags
    -Wl,–verbose
    -D_FORTIFY_SOURCE=2
    -D_GLIBCXX_ASSERTIONS
    -fasynchronous-unwind-tables
    -fexceptions
    -pipe
    -Werror=format-security
    -Werror=return-type
    -Wl,-z,defs
    -Wl,-z,now
    -Wl,-z,relro
    -fno-common
    -fopenmp
    -msse
    -msse3
    -mavx
    -march=native
    -fconcepts-diagnostics-depth=10
    # Build-type-related options
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-ggdb>
)
