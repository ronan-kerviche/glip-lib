<?php
// Check the post : 
//print_r($_POST);
// Other debug :
//error_reporting(-1);
//ini_set('display_errors', 'On');
//set_error_handler("var_dump");

$pageAbsoluteAddress = 'http://glip-lib.net/page14__contact_form.html';
if(isset($_POST['submit']))
{
	// Do some sanitization :
	$from = filter_var($_POST['email'], FILTER_SANITIZE_EMAIL);
	$name = filter_var($_POST['name'], FILTER_SANITIZE_STRING);
	$message = filter_var($_POST['message'], FILTER_SANITIZE_STRING);
	$to = file_get_contents('./Protected/contact.txt');
	$subject = "[glip-lib.net] Contact form.";
	$message = "From $name :\r\n$message";
	$headers = "From: $name <automaticform@glip-lib.net>\r\nReply-To: $from\r\nX-Mailer: PHP/" . phpversion();
	$returnCode = mail($to, $subject, $message, $headers);
	//echo "To : $to<br>";
	//echo "Subject : $subject<br>";
	//echo "Message : $message<br>";
	//echo "headers : $headers<br>";
	$page = file_get_contents($pageAbsoluteAddress);
	if($returnCode==TRUE)
	{
		// Send ok
		if(empty($from))
			$complimentary = "(Anonymous submission.)";
		else
			$complimentary = "Reply will be addressed to &lt;$from&gt;.";
		$message = "<div style=\"width:100%; text-align:center; background-color:#AFA; font-weight: bold; border-radius: 1em;\">Message Sent. $complimentary</div>";
	}
	else
		$message = "<div style=\"width:100%; text-align:center; background-color:#FAA; font-weight: bold; border-radius: 1em;\">Message could not be sent.</div>";
	// Replace with the right message within the page :
	$page = preg_replace("/<div id=\"placeholder\" style=\"visibility: hidden;\">.*<\/div>/", $message, $page);
	echo $page;
	
}
else
{
	// Non-permanent redirection :
	header('Location: ' . $pageAbsoluteAddress, true, 302);
   	die();
}
?>
