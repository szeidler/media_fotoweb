<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Session\AccountProxyInterface;

/**
 * Interface FotowebLoginManagerInterface.
 */
interface FotowebLoginManagerInterface {

  /**
   * Returns the Fotoweb Login Token for a Drupal account.
   *
   * @param \Drupal\Core\Session\AccountProxyInterface $user
   *   The user proxy object.
   *
   * @return string
   *   The Fotoweb Login Token.
   */
  public function getLoginTokenFromAccount(AccountProxyInterface $user);

}
