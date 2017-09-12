<?php 

/**
 * Create Ticket New
 */

function ticket_form($form, &$form_state){
	$form['ticket'] = array(
   '#type' => 'fieldset',
   '#title' => t('فرم ارسال تیکت'),
   );

	$form['ticket']['title'] = array(
   '#type' => 'textfield',
   '#title' => t('موضوع'),
   '#required' => TRUE, 
   );

	$form['ticket']['description'] = array(
   '#type' => 'textarea',
   '#title' => t('توضیحات'),
   );

	$form['ticket']['file'] = array(
   '#type' => 'file',
   '#title' => t('فایل های ضمیمه درخواست'),
   '#description' => t('فرمت فایل (.zip)'),
   );

	$form['ticket']['submit'] = array(
   '#type' => 'submit',
   '#value' => t('ارسال'),
   );

 return $form;
}

function ticket_form_validate($form, &$form_state) {
  $file = file_save_upload('file', array(
    'file_validate_extensions' => array('zip'), 
    ));
  if($file){
       $filename = "file-".time().".zip";
       $ftp = ftp_server($file->uri,$filename);
	  if($ftp){
           $form_state['storage']['file'] = $filename;
	  }else{
	      form_set_error('file', t('مشکل وجود دارد بعدا تلاش کنید.'));
	  }
  }
}

function ticket_form_submit($form, &$form_state){
  global $user;
  global $service_url;
 
  $url = $service_url['url']."/createticket";
  $file = isset($form_state['storage']['file']) ? $form_state['storage']['file'] : 'null';
  $options = array(
    'method' => 'POST',
    'data' => drupal_json_encode(array("Title"=>$form_state['values']['title'],"Description"=>$form_state['values']['description'],"AttachFile"=>"$file","Email"=>$user->mail)),
    'headers' => array('Content-Type' => 'application/json; charset=utf-8'),
    );
  $response = drupal_http_request($url, $options);
  //drupal_set_message(print_r($response,true));
  if(drupal_json_decode(@$response->data)){
      $data = drupal_json_decode(drupal_json_decode($response->data));
      if($data['Success']){
		 	drupal_set_message(t('درخواست شما با موفقیت ارسال شد<br> <b> کدرهگیری: %string</b>', array('%string' => $data['message']['TrackingCode'])));

      }else{
         drupal_set_message('پاسخی از مرکز دریافت نشد.','error');    
        }
  }else{
    
         drupal_set_message('مشکل در برقراری ارتباط موجود دارد  لطفا بعدا تلاش کنید.','error');    
  }
  
}

/**
 * Show Tickets
 */
function tickets_list(){
  global $user;
  global $service_url;

  $header = array(
    array('data' => t('موضوع')),   
    array('data' => t('تاریخ و ساعت ایجاد')),
    array('data' => t('وضعیت')),
    array('data' => t('کد رهگیری')),
    ); 

  $url = $service_url['url']."/showtickets";
  $options = array(
    'method' => 'POST',
    'data' => drupal_json_encode(array("Email"=>$user->mail)),
    'headers' => array('Content-Type' => 'application/json; charset=utf-8'),
    );

  $response = drupal_http_request($url, $options);
   if(drupal_json_decode(drupal_json_decode(@$response->data))){
      $data = drupal_json_decode(drupal_json_decode($response->data));
      if($data['Success']){
    		foreach($data["message"] as $key => $value){

              $rows[] = array('data' =>
               array(
                 l(t($value["Title"]), 'ticket/showticket/'.$value["TId"]),
                 t($value["Date"]),
                 t('<p class="status-answer">'.$value["Status"].'</p>'),
                 t($value["TrackingCode"]),
                 ),
               );
            }
      }else{
             drupal_set_message('مشکل در برقراری ارتباط موجود دارد  لطفا بعدا تلاش کنید.','error');    
      }
    }
    $per_page = 12;
    $current_page = pager_default_initialize(count($rows), $per_page);
    $chunks = array_chunk($rows, $per_page, TRUE);
    
    /*$build['table']  = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $chunks[$current_page],
      '#empty' => t('No bot available.'),
      );*/
  $build = theme('table', array('header' => $header, 'rows' => @$chunks[$current_page],'empty'=>t('No bot available.')));
  $build .= theme('pager', array('quantity',count($rows)));
  return $build;
}

 /**
 * Chat ticket
 */
 function ticket_chat($tid){

  global $user;
  global $service_url;

  $url = $service_url['url']."/showticket";
  $options = array(
    'method' => 'POST',
    'data' => drupal_json_encode(array("Email"=>$user->mail,"Tid"=>$tid)),
    'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );

  $response = drupal_http_request($url, $options);
  
     if(drupal_json_decode(drupal_json_decode(@$response->data))){
      $data = drupal_json_decode(drupal_json_decode($response->data));
      if($data['Success']){
           if($data['message']){
              $build = array();
              $data['tid'] = array('tid'=>$tid);
              $output = theme('newticket', array('data' => $data));
              
              $build['html'] = array(
               '#type' => 'markup',
               '#markup'=> $output,
               );
              $var = array('tid',$tid);
              $build['form'] =  drupal_get_form('SendReply_form',$var);
              return $build;
          }else{
                drupal_set_message('تیکتی با این شناسه وجود ندارد.','error'); 
                drupal_goto('/ticket/showtickets');

          }
      }else{
             drupal_set_message('مشکل در برقراری ارتباط موجود دارد  لطفا بعدا تلاش کنید.','error');    
      }
    }
 
  
  
}


 /**
 * Send Reply
 */

 function SendReply_form($form, &$form_state,$tid){
  $form['description1'] = array(
    '#type' => 'textarea',
    '#title' => t('توضیحات'),
    '#required' => false, 
    );

  $form['file1'] = array(
    '#type' => 'file',
    '#title' => t('فایل های ضمیمه درخواست'),
    '#required' => FALSE, 
    '#description' => t('فرمت فایل (.zip)')
    );
  $form['tid1'] = array(
    '#type' => 'hidden',
    '#required' => true,
    '#value'=> $tid[1]
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('ارسال'),
    '#submit' => array('SendReply_form_send_submit'),
    );
  $form['close'] = array(
    '#type' => 'submit',
    '#value' => t('خاتمه درخواست'),
    '#submit' => array('SendReply_form_close_submit'),
    );

  return $form;
}


function SendReply_form_send_submit($form, &$form_state){
  global $user;
  global $service_url;
  
   $file = file_save_upload('file1', array(
    'file_validate_extensions' => array('zip'), 
    ));
  if($file){
       $filename = "file-".time().".zip";
       $ftp = ftp_server($file->uri,$filename);
	  if($ftp){
          $form_state['storage']['file1'] = $filename;
	  }else{
	      form_set_error('file', t('مشکل وجود دارد بعدا تلاش کنید.'));
	  }
  }
  
  $text = $form_state['values']['description1'];
  $tid = $form_state['values']['tid1'];
  $file = isset($form_state['storage']['file1']) ? $form_state['storage']['file1'] : 'null';
  
  $url = $service_url['url']."sendreply";
  $options = array(
    'method' => 'POST',
    'data' => drupal_json_encode(array("Email"=>$user->mail,"Tid"=>$tid,'Description'=>$text,"AttachFile"=>$file)),
    'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );

  $response = drupal_http_request($url, $options);
    
   if(drupal_json_decode(drupal_json_decode(@$response->data))){
      $data = drupal_json_decode(drupal_json_decode($response->data));

      if($data['Success']){
            drupal_set_message('درخواست شما ارسال شد. منتظر پاسخ باشید.');
            
      }else{
             drupal_set_message('مشکل در برقراری ارتباط موجود دارد  لطفا بعدا تلاش کنید.','error');    
      }
    }
}

function SendReply_form_close_submit($form, &$form_state){
  global $user;
  global $service_url;
  $url = $service_url['url']."/EndTicket";
  $tid = $form_state['values']['tid1'];
  $options = array(
    'method' => 'POST',
    'data' => drupal_json_encode(array("Email"=>$user->mail,"Tid"=>$tid,"status"=>"خاتمه درخواست")),
    'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );

  $response = drupal_http_request($url, $options);
   if(drupal_json_decode(drupal_json_decode(@$response->data))){
      $data = drupal_json_decode(drupal_json_decode($response->data));

      if($data['Success']){
            drupal_set_message('درخواست شما بسته شد.');
            drupal_goto('/ticket/showtickets');

      }else{
             drupal_set_message('مشکل در برقراری ارتباط موجود دارد  لطفا بعدا تلاش کنید.','error');    
      }
    }

}


 /**
 * Config
 */

 function config_form($form, &$form_state){
   $result = db_select('urlticket', 'u')
   ->fields('u')
   ->execute()
   ->fetch();

   $form['reply'] = array(
    '#type' => 'fieldset',
    '#title' => t('تنظیم آدرس وب سرویس'),
    );

   $form['reply']['url'] = array(
    '#type' => 'textfield',
    '#title' => t('آدرس سرویس'),
    '#required' => true, 
    '#description' => t('http://example.com/'),
    '#default_value' => t($result->url),   
    );

   $form['reply']['record'] = array(
    '#type' => 'submit',
    '#value' => t('ذخیره'),
    '#submit' => array('config_form_record_submit'),
    );

   $form['reply']['test'] = array(
    '#type' => 'submit',
    '#value' => t('تست'),
    '#submit' => array('config_form_test_submit'),
    );

   return $form;
 }


 function config_form_record_submit($form, &$form_state){

  $url = $form_state['values']['url'];
  $update = db_update('urlticket')
  ->fields(array(
    'url' => $url
    ))
  ->execute();
  if($update){
    drupal_set_message('باموفقیت ثبت شد');
  }
}


function config_form_test_submit($form, &$form_state){

  $url = $form_state['values']['url'].'/testconnection';
  $options = array(
    'method' => 'POST',
    'data' => drupal_json_encode(array("Test"=>"Connection")),
    'headers' => array('Content-Type: text/html; charset=utf-8'),
    );

  $response = drupal_http_request($url, $options);
  drupal_set_message(print_r($response,true));
  if($response->code == 200){
       if ($response->data == "true"){
        drupal_set_message('اتصال با سرور برقرار شد');
      }
    }else{
      drupal_set_message(t('اتصال برقرار نشد.'),'error');
    }
}


 /**
 * Config
 */

 function ftp_config_form($form, &$form_state){
   $result = db_select('ftpticket', 'ft')
   ->fields('ft')
   ->execute()
   ->fetch();

   $form['reply'] = array(
    '#type' => 'fieldset',
    '#title' => t('تنظیم اف تی پی'),
    );

   $form['reply']['host'] = array(
    '#type' => 'textfield',
    '#title' => t('آدرس اف تی پی'),
    '#required' => true, 
    '#default_value' => t($result->host),   
    );

   $form['reply']['username'] = array(
    '#type' => 'textfield',
    '#title' => t('نام کاربری اف تی پی'),
    '#required' => true, 
    '#default_value' => t($result->username),   
    );
    
   $form['reply']['password'] = array(
    '#type' => 'textfield',
    '#title' => t('پسورد اف تی پی'),
    '#required' => true, 
    '#default_value' => t($result->password),   
    );
    
   $form['reply']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('آدرس دسترسی فایل'),
    '#required' => true, 
    '#description' => 'http://example.com/dowloand/',
    '#default_value' => t($result->path),   
   );
   $form['reply']['record'] = array(
    '#type' => 'submit',
    '#value' => t('ذخیره'),
    );


   return $form;
 }

function ftp_config_form_submit($form, &$form_state){

  $host = $form_state['values']['host'];
  $username = $form_state['values']['username'];
  $password = $form_state['values']['password'];
  $path = $form_state['values']['path'];

  $update = db_update('ftpticket')
  ->fields(array(
    'host' => $host,
    'username' => $username,
    'password' => $password,
    'path' => $path,
    ))
  ->execute();
  if($update){
    drupal_set_message('باموفقیت ثبت شد');
  }
}