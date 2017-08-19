<?php

/**
 * MediaBrowserPlugin to embed the Fotoweb Selection Widget into Media.
 */
class FotowebBrowser extends MediaBrowserPlugin {

  /**
   * {@inheritdoc}
   */
  public function access($account = NULL) {
    return file_entity_access('create', $account);
  }

  /**
   * {@inheritdoc}
   */
  public function view() {
    $embedded_selection_widget = $this->buildEmbeddedSelectionWidget();
    $build['selection_widget'] = array(
      '#type' => 'markup',
      '#markup' => $embedded_selection_widget,
      '#attached' => array(
        'js' => array(
          drupal_get_path('module', 'media_fotoweb') . '/js/media_fotoweb.js',
          array(
            'data' => array(
              'media_fotoweb' => array(
                'fotoweb_host' => variable_get('media_fotoweb_server', ''),
                'assetAddAjaxUrl' => base_path() . 'media-fotoweb/add/asset',
              ),
            ),
            'type' => 'setting',
          ),
        ),
      ),
    );
    return $build;
  }

  /**
   * Builds the Fotoweb Embedded Selection Widget as an <iframe>.
   *
   * @return string
   *   Returns the embedded widget markup.
   */
  protected function buildEmbeddedSelectionWidget() {
    $fotoweb_host = variable_get('media_fotoweb_server', '');
    $widget_url = $fotoweb_host . '/fotoweb/widgets/selection';
    $widget_height = variable_get('media_fotoweb_selection_widget_height', 500);

    // Generate user login token and append it to widget url, when using SSO.
    if (variable_get('media_fotoweb_selection_widget_use_sso', FALSE)) {
      module_load_include('inc', 'media_fotoweb', '/includes/media_fotoweb.helper');
      $fotoweb_username = _media_fotoweb_get_fotoweb_username();
      if (!empty($fotoweb_username) && $user_login_token = $this->getUserLoginToken($fotoweb_username)) {
        $widget_url .= '?lt=' . $user_login_token;
      }
    }

    // Create the widget using an <iframe>.
    $build = format_string('<iframe src="@widget_url" width="100%" height="@widget_height"></iframe>', array(
      '@widget_url' => $widget_url,
      '@widget_height' => $widget_height,
    ));

    return $build;
  }

  /**
   * Returns a login token for the given user for Single Sign-on.
   *
   * @param string $fotoweb_username
   *   Fotoweb username for authentication.
   *
   * @return string
   *   LoginToken to use for all HTTP requests.
   */
  protected function getUserLoginToken($fotoweb_username) {
    $fotoweb_encryption_secret = variable_get('media_fotoweb_encryption_secret', '');
    $token_generator = new FotowebLoginTokenGenerator($fotoweb_encryption_secret, TRUE);
    return $token_generator->CreateLoginToken($fotoweb_username);
  }

}
