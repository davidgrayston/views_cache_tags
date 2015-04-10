<?php

/**
 * Implements hook_views_plugins().
 */
function views_cache_tags_views_plugins() {
  return array(
    'cache' => array(
      'views_cache_tags' => array(
        'path' => drupal_get_path('module', 'views_cache_tags') . '/views',
        'title' => t("Cache Tags"),
        'help' => t("Add cache tags to query for invalidation."),
        'handler' => 'views_cache_tags_plugin',
        'uses options' => TRUE,
      ),
    ),
  );
}
