<?php 
global $user;
?>
<?php if($data['message']): ?>

<div id="ticket-min">
	<div class="block ticket-header">
		<div class="subject">
			<p> موضوع درخواست: <span> <?php print $data['message'][0]['Title'] ?> </span> </p>
		</div>
		<div class="tracking-code">
			<p> کدرهگیری: <span><?php print  $data['tid']['tid'] ?> </span>  </p>
		</div>
	</div>
	
	<?php foreach ($data['message'] as $key => $value) : ?>
	    <?php if($value['Type'] == 2): ?>
		<div class="ticket-operator">
			<div class="ex-operator">
				<div class="ticket-name">
					<i class="fa fa-user"></i><span> کارشناس </span>
				</div>
				<div class="ticket-date">
					<p>تاریخ و ساعت: <span><?php print  $value['time']." ".$value['date']?> </span></p>
				</div>
			</div>
			<div class="msg">
				<p> <span><?php print  $value['Description'] ?></span></p>
			    <?php if($value['AttachFile'] !='null'): ?>
				    <div class="attachments-ex"><p> فایل ضمیمه : <a href="http://ticket.wsmiran.ir/<?php print $value['AttachFile'] ?>"><?php print $value['AttachFile'] ?></a> </p></div>
				<?php endif; ?>
			</div>
	   </div>
	   <?php endif; ?>
	   <?php if($value['Type'] == 1): ?>
	   <div class="ticket-clinet">
			<div class="ex-clinet">
				<div class="ticket-name">
					<i class="fa fa-user"></i><span> نام کاربری:</span><span> <?php print $user->mail ?> </span>
				</div>
				<div class="ticket-date">
					<p>تاریخ و ساعت: <span><?php print  $value['time']." ".$value['date']?> </span></p>
				</div>
			</div>
			<div class="msg">
				<p><span><?php print  $value['Description'] ?> </span> </p>
			  <?php if($value['AttachFile']!='null'): ?>
				    <div class="attachments"><p> فایل ضمیمه : <a href="http://ticket.wsmiran.ir/<?php print $value['AttachFile'] ?>"><?php print $value['AttachFile'] ?></a> </p></div>
				<?php endif; ?>
			</div>
	   </div>
	   <?php endif; ?>
	 <?php endforeach; ?>
</div>
<?php endif; ?>