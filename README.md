# views_cache_tags

Concept:
- Generate view cache key using term cache tags
- Reset these cache tags when a node attached to the term is saved
- As a result, the query cache key will invalidate
- Also allows altering of tags

Current functionality:
- Extends time-based view cache plugin
