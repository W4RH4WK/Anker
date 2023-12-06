# Coding Style

- Adhere to existing coding style
- Use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
- Re-format only code that has been changed

## Naming

- Use *camel-case* as default (e.g. `configFile`, `loadConfig`)
- Capitalize constants, enum members, types, type parameters, concepts, and namespaces (e.g. `ConfigManager`, `ConfigPath`)
- Use *all-caps* for defines / macros along with a project specific prefix (e.g. `ANKER_DEBUG_BREAK`)
- Omit the `get` prefix for getters
- Use prefixes like `is` or `has` for predicates (e.g. `hasParent`)
- Function names should start with a verb as they *do* something (e.g. `loadConfig`)

Certain identifiers are prefixed to make their *impact* easily recognizable in code:
- Prefix global variables with `g_`
- Prefix non-public member variables with `m_`
- Prefix static member variables with `sm_`
- Prefix interface types with `I` (e.g. `IAudioServer`)

## Documentation

- Keep doc-comments concise; you may use Markdown but plaintext readability has priority
- Add overview doc-comments to types / functions that are large or common across the code-base
- Clearly state invariants and limitations in the overview doc-comment
- Start a doc-comment with the name of the type / function
- Consider using separators to group elements together

## Error Handling

This is experimental!

- Return `Anker::Status` to indicate success or failure
- Use output parameters for the actual result
    - Put output parameters the beginning of the parameter list
    - Consider `std::optional<T>` as output parameter for factory functions
- Log errors immediately when they emerge
- Memorize the following macros:
    - `ANKER_CHECK`
    - `ANKER_TRY`
    - `ANKER_DEFER`
