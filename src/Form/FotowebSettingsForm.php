<?php

namespace Drupal\media_fotoweb\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Messenger\MessengerTrait;
use GuzzleHttp\Exception\RequestException;

/**
 * Configure media_fotoweb settings for this site.
 */
class FotowebSettingsForm extends ConfigFormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'media_fotoweb_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'media_fotoweb.settings',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('media_fotoweb.settings');

    $form['server'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Fotoweb server'),
      '#description' => $this->t('Use the server address, including protocol, excluding relative paths (/fotoweb) and trailing slashes. Example: https://fotoweb.mydomain.no'),
      '#default_value' => $config->get('server'),
      '#required' => TRUE,
    );

    $form['full_api_key'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Full API Key'),
      '#description' => $this->t('This module is using a Full Server-to-server API Authentication. See <a href="@documentation_url" target="_blank">the Fotoweb documentation</a> for more information.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/Setting_the_API_key_in_the_Operations_Center')),
      '#default_value' => $config->get('full_api_key'),
      '#required' => TRUE,
    );

    $form['selection_widget_use_sso'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use SSO for the asset selection widget?'),
      '#description' => t('Enable this, when you want to use the <a href="@documentation_url" target="_blank">Single Sign-on</a> function to automatically authenticate your Drupal user to the Fotoweb widget. Users need to set their Fotoweb username in their Drupal user profile, to make the Single Sign-on work.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Integrating_FotoWeb_with_third-party_systems/User_Authentication_for_Embeddable_Widgets#Single_Sign_On_(SSO)_for_Widgets')),
      '#default_value' => $config->get('selection_widget_use_sso'),
    );

    $form['encryption_secret'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Encryption secret'),
      '#description' => $this->t('Single-Sign on requires an <a href="@documentation_url" target="_blank">encryption secret</a> for authenticating the users.', array('@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/05_Configuring_sites/Finding_and%2F%2For_changing_the_encryption_secret')),
      '#default_value' => $config->get('encryption_secret'),
      '#states' => array(
        'visible' => array(
          ':input[name="selection_widget_use_sso"]' => array('checked' => TRUE),
        ),
        'required' => array(
          ':input[name="selection_widget_use_sso"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['selection_widget_height'] = array(
      '#type' => 'number',
      '#title' => t('Selection Widget Height'),
      '#description' => t('Specify the height of the selection widget in pixels.'),
      '#default_value' => $config->get('selection_widget_height'),
    );

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('media_fotoweb.settings');
    $config->set('server', $form_state->getValue('server'));
    $config->set('full_api_key', $form_state->getValue('full_api_key'));
    $config->set('selection_widget_use_sso', $form_state->getValue('selection_widget_use_sso'));
    $config->set('encryption_secret', $form_state->getValue('encryption_secret'));
    $config->set('selection_widget_height', $form_state->getValue('selection_widget_height'));

    /** @var \Drupal\media_fotoweb\FotowebClient $client */
    $client = \Drupal::service('media_fotoweb.client');
    $clientConfiguration = [
      'baseUrl' => $form_state->getValue('server'),
      'apiToken' => $form_state->getValue('full_api_key'),
      'client_config' => ['allow_redirects' => FALSE],
    ];
    $client->createClientFromConfiguration($clientConfiguration);
    try {
      if ($rendition_service = $client->fetchRenditionService()) {
        $config->set('rendition_service', $rendition_service);
      }
      else {
        $this->messenger()
          ->addWarning($this->t('No rendition service found. Therefore you cannot fetch original images. You might need to check your Fotoweb server configuration.'));
      }
    }
    catch (RequestException $e) {
      $errorMessage = $e->getMessage();
      $this->messenger()
        ->addError($this->t('There was an networking error: @error_message', array('@error_message' => $errorMessage)));
    }
    catch (\Exception $e) {
      $errorMessage = $e->getMessage();
      $this->messenger()
        ->addError($this->t('@error_message', array('@error_message' => $errorMessage)));
    }

    $config->save();

    parent::submitForm($form, $form_state);
  }

}
