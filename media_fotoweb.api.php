<?php

/**
 * @file
 * Provides the Media Fotoweb API Hooks.
 */

/**
 * Alter guzzle configuration before initializing the client.
 *
 * @param array $configuration
 *   Guzzle configuration.
 */
function hook_media_fotoweb_guzzle_configuration_alter(&$configuration) {
  // For example set a proxy for the Guzzle client.
  $configuration['proxy'] = 'socks5://10.254.254.254:8123';
}
