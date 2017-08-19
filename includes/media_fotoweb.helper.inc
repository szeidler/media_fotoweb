<?php

/**
 * @file
 * Provides helper functions to interact with the Fotoweb API.
 */

/**
 * Returns the fotoweb username the from user field.
 *
 * @param StdClass|NULL $account
 *   Drupal account object.
 *
 * @return string
 *   Fotoweb username.
 */
function _media_fotoweb_get_fotoweb_username($account = NULL) {
  global $user;

  if (empty($account)) {
    $account = $user;
  }

  $account = user_load($account->uid);
  if (!empty($account->media_fotoweb_username[LANGUAGE_NONE][0]['value'])) {
    return $account->media_fotoweb_username[LANGUAGE_NONE][0]['value'];
  }

  return NULL;
}

/**
 * Extracts the fotoweb hash from hashed uri.
 *
 * @param $uri
 *   Fotoweb file URI.
 *
 * @return string|NULL
 *   Hash of the Fotoweb URI.
 */
function _media_fotoweb_extract_hash_from_uri($uri) {
  $parts = explode('/', $uri);
  $file_parts = explode('.', array_pop($parts));
  if (!empty($file_parts[0])) {
    return $file_parts[0];
  }
  return NULL;
}

/**
 * Try to find the matching file uri for the provided hash.
 *
 * @param $hash
 *   Hash of the Fotoweb URI.
 *
 * @return string|NULL
 *   Fotoweb URI returned by the database query.
 */
function _media_fotoweb_find_uri_by_hash($hash) {
  $result = db_query('SELECT f.uri FROM file_managed AS f WHERE SHA2(f.uri, 256) = :hash', array(':hash' => $hash))->fetchField();
  return $result;
}

/**
 * Overwrite image_style_deliver for fotoweb_media assets, because the common
 * image_style_deliver will always compare invalid tokens (Original uri vs.
 * image style path)
 *
 * @param string $style
 *   Image style name.
 * @param string $scheme
 *   Drupal file scheme.
 *
 * @return int
 *   Returns ACCESS denied on failure. Otherwise it's void.
 */
function _media_fotoweb_image_style_deliver($style, $scheme) {
  $args = func_get_args();
  array_shift($args);
  array_shift($args);
  $target = implode('/', $args);

  $valid = !empty($style);
  if (!variable_get('image_allow_insecure_derivatives', FALSE) || strpos(ltrim($target, '\/'), 'styles/') === 0) {
    // Use the original stream wrapper URI for token validation, as
    // image_style_url creates the token based on that.
    $hash = _media_fotoweb_extract_hash_from_uri($target);
    $original_file_uri = _media_fotoweb_find_uri_by_hash($hash);
    $valid = $valid && isset($_GET[IMAGE_DERIVATIVE_TOKEN]) && $_GET[IMAGE_DERIVATIVE_TOKEN] === image_style_path_token($style['name'], $original_file_uri);
  }

  if (!$valid) {
    return MENU_ACCESS_DENIED;
  }

  $image_uri = 'public://' . $target;
  $derivative_uri = image_style_path($style['name'], $image_uri);

  // Don't start generating the image if the derivative already exists or if
  // generation is in progress in another thread.
  $lock_name = 'image_style_deliver:' . $style['name'] . ':' . drupal_hash_base64($image_uri);
  if (!file_exists($derivative_uri)) {
    $lock_acquired = lock_acquire($lock_name);
    if (!$lock_acquired) {
      // Tell client to retry again in 3 seconds. Currently no browsers
      // are known to support Retry-After.
      drupal_add_http_header('Status', '503 Service Unavailable');
      drupal_add_http_header('Retry-After', 3);
      print t('Image generation in progress. Try again shortly.');
      drupal_exit();
    }
  }

  // Try to generate the image, unless another thread just did it while we were
  // acquiring the lock.
  $success = file_exists($derivative_uri) || image_style_create_derivative($style, $image_uri, $derivative_uri);

  if (!empty($lock_acquired)) {
    lock_release($lock_name);
  }

  if ($success) {
    $image = image_load($derivative_uri);
    file_transfer($image->source, array('Content-Type' => $image->info['mime_type'], 'Content-Length' => $image->info['file_size']));
  }
  else {
    watchdog('image', 'Unable to generate the derived image located at %path.', array('%path' => $derivative_uri));
    drupal_add_http_header('Status', '500 Internal Server Error');
    print t('Error generating image.');
    drupal_exit();
  }
}

/**
 * Copy of image_style_path() for use with fotoweb images.
 **
 * @param $style_name
 *   Image style name.
 * @param $uri
 *   Drupal file URI.
 *
 * @return string
 *   Built image style path.
 */
function _media_fotoweb_image_style_path($style_name, $uri) {
  // Force the image style to be returned with the default file scheme, but
  // with the file's original scheme in the path.
  return file_default_scheme() . '://styles/' . $style_name . '/' . file_default_scheme() . '/' . file_uri_target($uri);
}
