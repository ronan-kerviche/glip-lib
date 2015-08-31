<?php
/* Check the post : print_r($_POST); */

$pageAbsoluteAddress = 'http://glip-lib.net/page14__contact_form.html';
if(isset($_POST['submit']))
{
	/* Do some sanitization : */
	$from = filter_var($_POST['email'], FILTER_SANITIZE_EMAIL);
	$subject = filter_var($_POST['name'], FILTER_SANITIZE_STRING);
	$message = filter_var($_POST['message'], FILTER_SANITIZE_STRING);
	$to = file_get_contents('./Protected/contact.txt');
	$headers = "From:" . $from;
	mail($to, $subject, $message, $headers);
	/* Send ok */
	if(empty($from))
		$complimentary = "(Anonymous submission.)";
	else
		$complimentary = "Reply will be addressed to &lt;$from&gt;.";
	$page = file_get_contents($pageAbsoluteAddress);
	$page = preg_replace("/<div id=\"placeholder\" style=\"visibility: hidden;\">.*<\/div>/", "<div style=\"width:100%; text-align:center; background-color:#AFA; font-weight: bold; border-radius: 1em;\">Message Sent. $complimentary</div>", $page);
	echo $page;
}
else
{
	/* Non-permanent redirection : */
	header('Location: ' . $pageAbsoluteAddress, true, 302);
   	die();
}
?>
