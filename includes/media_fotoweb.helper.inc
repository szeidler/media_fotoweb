<?php

/**
 * Returns the fotoweb username the from user field.
 *
 * @param null $account
 *
 * @return string
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
