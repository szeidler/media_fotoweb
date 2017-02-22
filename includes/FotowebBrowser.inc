<?php

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
        ),
      ),
    );
    return $build;
  }

  protected function buildEmbeddedSelectionWidget() {
    $fotoweb_host = variable_get('media_fotoweb_server', '');
    $user_login_token = $this->getUserLoginToken();

    $build = format_string('<iframe src="@fotoweb_host/fotoweb/widgets/selection?lt=@login_token" width="100%" height="500" />', array(
      '@fotoweb_host' => $fotoweb_host,
      '@login_token' => $user_login_token
    ));

    return $build;
  }

  protected function getUserLoginToken() {
    $fotoweb_full_api_token = variable_get('media_fotoweb_full_api_token', '');
    $tokenGenerator = new FotowebLoginTokenGenerator($fotoweb_full_api_token, TRUE);
    return $tokenGenerator->CreateLoginToken('apiramsalt');
  }

}
