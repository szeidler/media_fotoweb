<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Session\AccountProxyInterface;
use Drupal\user\UserInterface;
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
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * Constructs a new FotowebLoginManager object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   Config factory service.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity manager.
   */
  public function __construct(ConfigFactoryInterface $config_factory, EntityTypeManagerInterface $entity_type_manager) {
    $this->configFactory = $config_factory;
    $this->entityTypeManager = $entity_type_manager;
  }

  /**
   * {@inheritdoc}
   */
  public function getLoginTokenFromAccount(AccountProxyInterface $account) {
    $fotoweb_username = NULL;
    if ($user = $this->getUserFromAccount($account)) {
      $fotoweb_username = $this->getFotowebUsernameFromUser($user);
    }
    return $this->getUserLoginTokenFromUsername($fotoweb_username);
  }

  /**
   * Returns the user from a given account.
   *
   * @param \Drupal\Core\Session\AccountProxyInterface $account
   *   The given user account.
   *
   * @return \Drupal\user\UserInterface|null
   *   The user entity.
   */
  protected function getUserFromAccount(AccountProxyInterface $account) {
    return $this->entityTypeManager->getStorage('user')->load($account->id());
  }

  /**
   * Returns the Fotoweb user name from a given user.
   *
   * @param \Drupal\user\UserInterface $user
   *   The given user.
   */
  protected function getFotowebUsernameFromUser(UserInterface $user) {
    $fotoweb_username = NULL;
    $config = $this->configFactory->get('media_fotoweb.settings');
    $sso_user_field = $config->get('sso_user_field');
    if (!empty($sso_user_field) && $user->hasField($sso_user_field)) {
      $fotoweb_username = $user->get($sso_user_field)->value;
    }

    return $fotoweb_username;
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
