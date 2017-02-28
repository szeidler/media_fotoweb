<?php

/**
 * Implements hook_file_formatter_info().
 */
function media_fotoweb_file_formatter_info() {
  $formatters = array();

  $formatters['media_fotoweb_image'] = array(
    'label' => t('Fotoweb image'),
    'file types' => array('image'),
    'default settings' => array(
      'image_style' => '',
    ),
    'view callback' => 'media_fotoweb_file_formatter_media_fotoweb_view',
    'settings callback' => 'media_fotoweb_file_formatter_media_fotoweb_settings',
  );

  return $formatters;
}

/**
 * Implements hook_file_formatter_FORMATTER_view().
 */
function media_fotoweb_file_formatter_media_fotoweb_view($file, $display, $langcode) {
  $scheme = file_uri_scheme($file->uri);
  if ($scheme == 'fotoweb') {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $image_style = $display['settings']['image_style'];
    $valid_image_styles = image_style_options(FALSE);

    if (empty($image_style) || !isset($valid_image_styles[$image_style])) {
      $element = array(
        '#theme' => 'image',
        '#path' => str_replace('http:', '', $wrapper->getImagePath()),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }
    else {
      $element = array(
        '#theme' => 'image_style',
        '#style_name' => $image_style,
        '#path' => $wrapper->getImagePath(),
        '#alt' => isset($file->override['attributes']['alt']) ? $file->override['attributes']['alt'] : $file->filename,
      );
    }

    return $element;
  }
}

/**
 * Implements hook_file_formatter_FORMATTER_settings().
 */
function media_fotoweb_file_formatter_media_fotoweb_settings($form, &$form_state, $settings) {
  $element = array();

  $element['image_style'] = array(
    '#title' => t('Image style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#default_value' => $settings['image_style'],
    '#empty_option' => t('None (original image)'),
  );

  return $element;
}
