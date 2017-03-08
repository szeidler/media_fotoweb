<?php

use GuzzleHttp\Psr7;
use GuzzleHttp\Exception\RequestException;

/**
 * Builds the Media Fotoweb admin settings form.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function media_fotoweb_admin_settings($form, &$form_state) {
  $form['media_fotoweb_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Fotoweb server'),
    '#description' => t('Use the server address, including protocol, excluding relative paths (/fotoweb) and trailing slashes. Example: https://fotoweb.mydomain.no'),
    '#default_value' => variable_get('media_fotoweb_server'),
    '#required' => TRUE,
  );
  $form['media_fotoweb_full_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Fotoweb Full API Key'),
    '#description' => t('This module is using a Full Server-to-server API Authentication. See <a href="@documentation_url" target="_blank">the Fotoweb documentation</a> for more information.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/Setting_the_API_key_in_the_Operations_Center')),
    '#default_value' => variable_get('media_fotoweb_full_api_key'),
    '#required' => TRUE,
  );
  $form['media_fotoweb_selection_widget_use_sso'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use SSO for the asset selection widget?'),
    '#description' => t('Enable this, when you want to use the <a href="@documentation_url" target="_blank">Single Sign-on</a> function to automatically authenticate your Drupal user to the Fotoweb widget. Users need to set their Fotoweb username in their Drupal user profile, to make the Single Sign-on work.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Integrating_FotoWeb_with_third-party_systems/User_Authentication_for_Embeddable_Widgets#Single_Sign_On_(SSO)_for_Widgets')),
    '#default_value' => variable_get('media_fotoweb_selection_widget_use_sso'),
  );
  $form['media_fotoweb_encryption_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Fotoweb encryption secret'),
    '#description' => t('Single-Sign on requires an <a href="@documentation_url" target="_blank">encryption secret</a> for authenticating the users.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/05_Configuring_sites/Finding_and%2F%2For_changing_the_encryption_secret')),
    '#default_value' => variable_get('media_fotoweb_encryption_secret'),
    '#states' => array(
      'visible' => array(
        ':input[name="media_fotoweb_selection_widget_use_sso"]' => array('checked' => TRUE),
      ),
      'required' => array(
        ':input[name="media_fotoweb_selection_widget_use_sso"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['media_fotoweb_local_file_storage_type'] = array(
    '#type' => 'select',
    '#title' => t('File storage type'),
    '#description' => t('Original images from Fotoweb might be unnecessary big for your website usage. You can either store the original image or a smaller appropriate preview with your desired maximum size.'),
    '#options' => array(
      'original' => t('Original image'),
      'preview' => t('Preview'),
    ),
    '#default_value' => variable_get('media_fotoweb_local_file_storage_type', 'preview'),
  );
  $form['media_fotoweb_local_file_max_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum local file size'),
    '#description' => t('Define the maximal size for your locally stored images. The module will import the appropriate preview size from Fotoweb.'),
    '#default_value' => variable_get('media_fotoweb_local_file_max_size', 1920),
    '#states' => array(
      'visible' => array(
        ':input[name="media_fotoweb_local_file_storage_type"]' => array('value' => 'preview'),
      ),
      'required' => array(
        ':input[name="media_fotoweb_local_file_storage_type"]' => array('value' => 'preview'),
      ),
    ),
  );

  return system_settings_form($form);
}

/**
 * Validate settings form and test successful authentication to the Fotoware API
 * endpoint.
 *
 * @param $form
 * @param $form_state
 */
function media_fotoweb_admin_settings_validate($form, $form_state) {
  // Make the encryption secret field required, when having SSO enabled.
  if (!empty($form_state['values']['media_fotoweb_selection_widget_use_sso']) && empty($form_state['values']['media_fotoweb_encryption_secret'])) {
    form_error($form['media_fotoweb_encryption_secret'], t('@field_name field is required.', array(
      '@field_name' => $form['media_fotoweb_encryption_secret']['#title'],
    )));
  }
  // Make the maximum local file size field required, when having preview mode enabled.
  if ($form_state['values']['media_fotoweb_local_file_storage_type'] === 'preview' && empty($form_state['values']['media_fotoweb_local_file_max_size'])) {
    form_error($form['media_fotoweb_local_file_max_size'], t('@field_name field is required.', array(
      '@field_name' => $form['media_fotoweb_local_file_max_size']['#title'],
    )));
  }

  // Test, that Fotoweb Endpoint is accessible with the saved configuration.
  $server = $form_state['values']['media_fotoweb_server'];
  $full_api_key = $form_state['values']['media_fotoweb_full_api_key'];
  $fotoweb = new FotowebBase($server, $full_api_key);

  // Perform authentication and validate its response.
  try {
    $response = $fotoweb->authenticate();
    // No errors and exceptions. Display a success message.
    drupal_set_message(t('The connection to the Fotoweb endpoint was established succesfully.'));
  } catch (RequestException $e) {
    if ($e->hasResponse()) {
      $error_message = (string) Psr7\str($e->getResponse());
    }
    else {
      $error_message = $e->getMessage();
    }
    form_error($form['media_fotoweb_server'], t('There was an networking error: @error_message', array('@error_message' => $error_message)));
  } catch (Exception $e) {
    $error_message = $e->getMessage();
    if ($e->getCode() === 403) {
      form_error($form['media_fotoweb_full_api_key'], t('There was an error: @error_message', array('@error_message' => $error_message)));
    }
    else {
      form_error($form['media_fotoweb_server'], t('There was an error: @error_message', array('@error_message' => $error_message)));
    }
  }
}
