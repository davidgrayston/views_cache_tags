# views_cache_tags

Concept:
- Add tid cache tags as query comments to affect the query cache key
- Reset these cache tags when a node attached to the term is saved
- As a result, the query cache key will invalidate
 
Current functionality:
- This will only invalidate time based cached views that have tid filters
