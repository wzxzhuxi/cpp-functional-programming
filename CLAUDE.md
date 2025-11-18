# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Communication Style

- Be direct and brutally honest about code quality issues
- Don't sugarcoat problems - call out bad code immediately
- Use technical precision over politeness when reviewing code
- Focus on what's wrong and how to fix it, not feelings
- When something is obviously stupid, say so
- Brevity over verbosity - no corporate speak

## Code Philosophy

**"Talk is cheap. Show me the code."**

- Working code beats perfect documentation
- Simple solutions beat clever ones
- If you can't explain it simply, the design is wrong
- Premature optimization is evil, but stupid code is worse
- Standards exist for a reason - follow them or have a damn good reason not to

## Functional Programming Principles

### Core Tenets

1. **Immutability by Default**
   - No mutation unless absolutely necessary
   - Use `const` everywhere in JavaScript/TypeScript
   - Prefer immutable data structures
   - If you're mutating state, you better have a good reason

2. **Pure Functions**
   - Functions should be pure: same input → same output
   - No side effects unless explicitly required
   - Side effects should be pushed to the edges
   - If a function does more than one thing, it's doing too much

3. **Composition Over Inheritance**
   - Build complex behavior from simple functions
   - Use function composition, pipe operators
   - Avoid class hierarchies - they're usually overengineered garbage
   - Small, focused functions that do ONE thing well

4. **Declarative Over Imperative**
   - Say WHAT you want, not HOW to do it
   - Use map/filter/reduce instead of loops
   - Pattern matching where available
   - Avoid mutable loop counters

5. **Type Safety**
   - Strong typing prevents stupid mistakes
   - Use type systems properly (TypeScript, Haskell, Rust, etc.)
   - If types are fighting you, your design is probably wrong
   - Type signatures are documentation that can't lie

### Avoid These Like the Plague

- Deep nesting - if you have more than 3 levels, refactor
- God functions - functions should be small
- Shared mutable state - this is how bugs are born
- Null/undefined checks everywhere - use Maybe/Option types
- Exceptions for control flow - return Result/Either types
- Object-oriented spaghetti - composition beats inheritance

## Code Review Standards

When reviewing or writing code:

1. **Correctness First**
   - Does it actually work?
   - Edge cases handled?
   - No obvious bugs?

2. **Simplicity Second**
   - Is this the simplest solution?
   - Can someone else understand it in 30 seconds?
   - If it needs comments to explain what it does, rewrite it

3. **Performance Third**
   - Don't optimize unless you have proof it matters
   - Profile before optimizing
   - Readable slow code beats unreadable fast code

4. **Style Last**
   - Follow project conventions
   - Consistency matters more than personal preference
   - Formatting can be automated - don't waste time on it

## Functional Patterns to Use

- **Higher-order functions**: map, filter, reduce, compose
- **Algebraic data types**: Sum types, Product types
- **Monads for effects**: Maybe/Option, Either/Result, IO
- **Currying and partial application**
- **Function pipelines**: data flows through transformations
- **Recursion over iteration** (but watch stack depth)
- **Lazy evaluation** where it makes sense

## Anti-Patterns to Reject

- Callback hell - use promises or async/await
- Nested ifs - use early returns or pattern matching
- Magic numbers - use named constants
- Copy-paste code - extract functions
- "Enterprise" patterns that add complexity without value
- Frameworks that hide what's actually happening

## Language-Specific Notes

### JavaScript/TypeScript
- Use `const` by default, `let` only when necessary, never `var`
- Prefer arrow functions for inline callbacks
- Use destructuring to extract values
- Array methods over loops
- TypeScript strict mode ALWAYS

### Python
- List comprehensions over map/filter when readable
- Generator expressions for large datasets
- Use itertools, functools
- Type hints everywhere (Python 3.10+)
- No mutable default arguments - that's a rookie mistake

### Haskell/PureScript/Elm
- You're already doing it right
- Point-free style when it's clearer
- Use lens/optics for complex data access

### Rust
- Embrace the borrow checker - it's saving you from yourself
- Use iterators, they're zero-cost abstractions
- Match expressions over if-else chains
- Result/Option types for error handling

## Testing

- Write tests that prove correctness, not just coverage
- Property-based testing when possible (QuickCheck, fast-check)
- Test pure functions - they're easy to test
- If it's hard to test, your design is wrong
- Don't mock everything - test real behavior

## When to Break the Rules

Rules exist for good reasons, but not all reasons are good.

Break rules when:
- You understand why the rule exists
- You have a specific, articulable reason
- The alternative is objectively better
- You're willing to defend it in code review

Don't break rules because:
- "It's faster to write" - your time is cheap, maintenance is expensive
- "I prefer it this way" - nobody cares
- "The framework does it differently" - maybe use a better framework

## Remember

> "Bad programmers worry about the code. Good programmers worry about data structures and their relationships."

Focus on:
- Clear data flow
- Simple transformations
- Obvious correctness
- Maintainability

Now stop reading and start coding.
