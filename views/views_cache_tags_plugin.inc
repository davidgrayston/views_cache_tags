<?php

/**
 * @file
 * Views cache tags plugin.
 */

/**
 * Views caching using cache tags to generate key.
 *
 * @see views_plugin_cache()
 */
class views_cache_tags_plugin extends views_plugin_cache_time {

  /**
   * Implements views_plugin_cache#get_output_key().
   */
  function get_output_key() {
    return parent::get_output_key() . ':' . $this->get_cache_tags_key();
  }

  /**
   * Implements views_plugin_cache#get_results_key().
   */
  function get_results_key() {
    return parent::get_results_key() . ':' . $this->get_cache_tags_key();
  }

  /**
   * Return cache tags.
   */
  function get_cache_tags() {
    $query = $this->view->query;

    // Extract tids from query.
    $tids = array();
    $tids = array_merge($tids, $this->extract_tids_from_joins($query));
    $tids = array_merge($tids, $this->extract_tids_from_where($query));

    // Ensure all values are integers.
    $tids = array_map('intval', $tids);

    // Remove duplicates.
    $tids = array_unique($tids);

    // Add tid cache tags to query to affect the query cache key.
    $tags = array();
    foreach (array_unique($tids) as $tid) {
      $tags['taxonomy_term'][$tid] = $tid;
    }

    // Allow modules to alter view cache tags.
    $context = array(
      'view' => $this->view,
    );
    drupal_alter('views_cache_tags_tags', $tags, $context);

    // Convert entity ids into cache tags.
    $cache_tags = array();
    foreach ($tags as $entity_type => $entity_ids) {
      foreach ($entity_ids as $entity_id) {
        $cache_tags[$entity_type][$entity_id] = views_cache_tags_entity_cache_tag($entity_id, $entity_type);
      }
    }

    return $cache_tags;
  }

  /**
   * Get cache tags key.
   */
  function get_cache_tags_key() {
    return md5(serialize($this->get_cache_tags()));
  }

  /**
   * Extract tids from joins.
   */
  function extract_tids_from_joins($query) {
    $tids = array();
    // Discover terms filtering this view.
    foreach ($query->table_queue as $alias => $info) {
      // Grab the tids from taxonomy_index joins.
      if (isset($info['table']) && $info['table'] == 'taxonomy_index') {
        if (isset($info['join']->extra)) {
          foreach ($info['join']->extra as $on_clause) {
            if (isset($on_clause['field']) && $on_clause['field'] == 'tid') {
              if (isset($on_clause['value'])) {
                if (is_array($on_clause['value'])) {
                  $tids = array_merge($tids, $on_clause['value']);
                }
                else {
                  $tids[] = $on_clause['value'];
                }
              }
            }
          }
        };
      }
    }
    return array_unique($tids);
  }

  /**
   * Extract tids from where clause.
   */
  function extract_tids_from_where($query) {
    $tids = array();
    if (isset($query->where)) {
      foreach ($query->where as $cond_items) {
        if (isset($cond_items['conditions']) && is_array($cond_items['conditions'])) {
          foreach ($cond_items['conditions'] as $cond) {
            if (isset($cond['field']) && substr($cond['field'], -3) == 'tid') {
              // Get tid from basic where clause.
              if (is_array($cond['value'])) {
                $tids = array_merge($tids, $cond['value']);
              }
              else {
                $tids[] = $cond['value'];
              }
            }
            elseif (isset($cond['field']) && substr($cond['field'], -3) == 'nid') {
              // Extract tids from node sub queries.
              if (is_object($cond['value']) && $cond['value'] instanceof SelectQuery) {
                $sq_cond_items = $cond['value']->conditions();
                foreach ($sq_cond_items as $sq_cond_item) {
                  if (isset($sq_cond_item['field']) && is_object($sq_cond_item['field']) && $sq_cond_item['field'] instanceof DatabaseCondition) {
                    $sq_conds = $sq_cond_item['field']->conditions();
                    foreach ($sq_conds as $sq_cond) {
                      if (isset($sq_cond['field']) && substr($sq_cond['field'], -3) == 'tid') {
                        if (is_array($sq_cond['value'])) {
                          $tids = array_merge($tids, $sq_cond['value']);
                        }
                        else {
                          $tids[] = $sq_cond['value'];
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    return array_unique($tids);
  }
}
