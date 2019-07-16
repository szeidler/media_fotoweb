<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Session\AccountInterface;
use Fotoweb\Authentication\LoginTokenGenerator;

/**
 * Class FotowebLoginManager.
 */
class FotowebLoginManager implements FotowebLoginManagerInterface {

  /**
   * Config factory service.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The current user.
   *
   * @var \Drupal\Core\Session\AccountInterface
   */
  protected $currentUser;

  /**
   * Constructs a new FotowebLoginManager object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   Config factory service.
   * @param \Drupal\Core\Session\AccountInterface $current_user
   *   The current user.
   */
  public function __construct(ConfigFactoryInterface $config_factory, AccountInterface $current_user) {
    $this->configFactory = $config_factory;
    $this->currentUser = $current_user;
  }

  /**
   * {@inheritdoc}
   */
  public function getLoginTokenFromUser(AccountInterface $user) {
    // TODO: Take it from a user field.
    $fotoweb_username = 'apiramsalt';
    return $this->getUserLoginTokenFromUsername($fotoweb_username);
  }

  /**
   * Returns a login token for the given user for Single Sign-on.
   *
   * @param string $username
   *   Fotoweb username for authentication.
   *
   * @return string
   *   LoginToken to use for all HTTP requests.
   */
  protected function getUserLoginTokenFromUsername($username) {
    $config = $this->configFactory->get('media_fotoweb.settings');

    $encryption_secret = $config->get('encryption_secret');
    $token_generator = new LoginTokenGenerator($encryption_secret, TRUE);
    return $token_generator->CreateLoginToken($username);
  }

}
