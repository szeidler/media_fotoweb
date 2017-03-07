<?php

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
    '#description' => t('Enable this, when you want to use the <a href="@documentation_url" target="_blank">Single Sign-on</a> function to automatically authenticate your Drupal user to the Fotoweb widget.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Integrating_FotoWeb_with_third-party_systems/User_Authentication_for_Embeddable_Widgets#Single_Sign_On_(SSO)_for_Widgets')),
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

  return system_settings_form($form);
}

function media_fotoweb_admin_settings_validate($form, $form_state) {
  if (!empty($form_state['values']['media_fotoweb_selection_widget_use_sso']) && empty($form_state['values']['media_fotoweb_encryption_secret'])) {
    form_error($form['media_fotoweb_encryption_secret'], t('@field_name field is required.', array(
      '@field_name' => $form['media_fotoweb_encryption_secret']['#title'],
    )));
  }
}
