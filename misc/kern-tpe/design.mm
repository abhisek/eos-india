<map version="0.8.0">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1182983257493" ID="Freemind_Link_1746713972" MODIFIED="1182979696169" TEXT="Trusted Path &#xa;Execution (LKM)">
<node CREATED="1182915946109" ID="Freemind_Link_39618526" MODIFIED="1182915948952" POSITION="left" TEXT="Objectives">
<node CREATED="1182915950274" ID="Freemind_Link_490276694" MODIFIED="1182979528785" TEXT="Prevent execution of insecure programs in insecure path"/>
<node CREATED="1182916017674" ID="Freemind_Link_1494332235" MODIFIED="1182916033652" TEXT="Prevent execution of insecure programs through LD tricks"/>
<node CREATED="1182980123416" ID="Freemind_Link_584613720" MODIFIED="1182983757495" TEXT="100% Kernel Code as a Loadable Kernel Module"/>
<node CREATED="1182980137725" ID="Freemind_Link_681980044" MODIFIED="1182984467860" TEXT="Avoid arch dependent code as much as possible"/>
<node CREATED="1182980305633" ID="Freemind_Link_1248708637" MODIFIED="1182980317998" TEXT="Should be configurable at Runtime by super user">
<node CREATED="1182980320933" ID="Freemind_Link_956891532" MODIFIED="1182980329715" TEXT="Strict mode should not allow dynamic configuration"/>
<node CREATED="1182980330643" ID="Freemind_Link_797372133" MODIFIED="1182980335862" TEXT="Non-strict mode should allow"/>
</node>
</node>
<node CREATED="1182979449487" ID="Freemind_Link_848373615" MODIFIED="1182990851828" POSITION="left" TEXT="Production Features">
<node CREATED="1182979463861" ID="Freemind_Link_1135840171" MODIFIED="1182979465707" TEXT="Logging">
<node CREATED="1182979466494" ID="Freemind_Link_1004058101" MODIFIED="1182979471841" TEXT="Loggin APIs"/>
<node CREATED="1182979472861" ID="Freemind_Link_454070832" MODIFIED="1182979476869" TEXT="Logging to klogd"/>
<node CREATED="1182979478034" ID="Freemind_Link_1407881423" MODIFIED="1182980234168" TEXT="Logging to TTY"/>
<node CREATED="1182980247054" ID="Freemind_Link_387416999" MODIFIED="1182980249511" TEXT="Debug Logging"/>
</node>
<node CREATED="1182979485861" ID="Freemind_Link_1397064749" MODIFIED="1182979487784" TEXT="Configuration">
<node CREATED="1182979488434" ID="Freemind_Link_1602012727" MODIFIED="1182979493561" TEXT="Read configuration from file"/>
<node CREATED="1182979508724" ID="Freemind_Link_586205348" MODIFIED="1182979514546" TEXT="Strict mode/Non strict mode"/>
<node CREATED="1182979547023" ID="Freemind_Link_271491228" MODIFIED="1182979735075" TEXT="Group Policy with optional path">
<node CREATED="1182979571083" ID="Freemind_Link_1290929082" MODIFIED="1182979611392" TEXT="Example: echo &quot;%100:/home/admin/backup.sh&quot; &gt; /proc/tpe/add"/>
</node>
<node CREATED="1182979552173" ID="Freemind_Link_712604724" MODIFIED="1182979740869" TEXT="User Policy with optional path">
<node CREATED="1182979624562" ID="Freemind_Link_800664626" MODIFIED="1182979637799" TEXT="Example: echo &quot;1000:/home/admin/backup.sh&quot; &gt; /proc/tpe/add"/>
</node>
<node CREATED="1182982539254" ID="Freemind_Link_1991633392" MODIFIED="1182982544973" TEXT="Single Proc interface for ACL">
<node CREATED="1182982545984" ID="Freemind_Link_948251346" MODIFIED="1182982574127" TEXT="To Add: echo &quot;+100:/home/admin/backup.sh&quot; &gt; /proc/tpe/acl"/>
<node CREATED="1182982575724" ID="Freemind_Link_693165826" MODIFIED="1182982591527" TEXT="To Del: echo &quot;-100:/home/admin/backup.sh&quot; &gt; /proc/tpe/acl"/>
<node CREATED="1183369648029" ID="Freemind_Link_765646587" MODIFIED="1183369664071" TEXT="To Del All: echo &quot;-*&quot; &gt; /proc/tpe/acl"/>
</node>
<node CREATED="1182990863466" ID="Freemind_Link_346209420" MODIFIED="1182990870616" TEXT="Single Proc interface for Control">
<node CREATED="1182990871366" ID="Freemind_Link_1809332144" MODIFIED="1182990894081" TEXT="Enable: echo 1 &gt; /proc/tpe/ctl"/>
<node CREATED="1182990895226" ID="Freemind_Link_1861807197" MODIFIED="1182990901845" TEXT="Disable: echo 0 &gt; /proc/tpe/ctl"/>
<node CREATED="1182990902786" ID="Freemind_Link_1378382385" MODIFIED="1182990914192" TEXT="Strict Mode: echo -1 &gt; /proc/tpe/ctl"/>
</node>
</node>
<node CREATED="1183369870712" ID="Freemind_Link_1061870475" MODIFIED="1183369873707" TEXT="Components">
<node CREATED="1183369874732" ID="Freemind_Link_1883667090" MODIFIED="1183369875929" TEXT="main"/>
<node CREATED="1183369876432" ID="Freemind_Link_1674234941" MODIFIED="1183369880369" TEXT="proc interface"/>
<node CREATED="1183369880913" ID="Freemind_Link_232712031" MODIFIED="1183369907334" TEXT="acl string processors"/>
<node CREATED="1183369891281" ID="Freemind_Link_711950468" MODIFIED="1183369898102" TEXT="acl list handlers"/>
<node CREATED="1183369911580" ID="Freemind_Link_1193367084" MODIFIED="1183369916749" TEXT="logging apis"/>
<node CREATED="1183375559751" ID="Freemind_Link_1450718993" MODIFIED="1183375563658" TEXT="conf file processor"/>
</node>
</node>
<node CREATED="1182979705791" ID="Freemind_Link_509827745" MODIFIED="1182979708545" POSITION="right" TEXT="Theory">
<node CREATED="1182983286813" ID="_" MODIFIED="1182983291363" TEXT="Subject">
<node CREATED="1182983298335" ID="Freemind_Link_292034602" MODIFIED="1182983299733" TEXT="Type">
<node CREATED="1182983311958" ID="Freemind_Link_545141905" MODIFIED="1182983314409" TEXT="User"/>
<node CREATED="1182983314704" ID="Freemind_Link_605931133" MODIFIED="1182983316387" TEXT="Group"/>
</node>
<node CREATED="1182983300185" ID="Freemind_Link_40532868" MODIFIED="1182983301447" TEXT="Secure">
<node CREATED="1182983327063" ID="Freemind_Link_1559603379" MODIFIED="1182915975493" TEXT="User: Root"/>
<node CREATED="1182983328945" ID="Freemind_Link_340589831" MODIFIED="1182983334609" TEXT="User in ACL allowed list"/>
<node CREATED="1182915982104" ID="Freemind_Link_343856838" MODIFIED="1182915987798" TEXT="Group in ACL allowed list"/>
</node>
<node CREATED="1182983301875" ID="Freemind_Link_1253891666" MODIFIED="1182983303849" TEXT="Insecure">
<node CREATED="1182983341813" ID="Freemind_Link_1569665909" MODIFIED="1182983344502" TEXT="Everybody else"/>
</node>
</node>
<node CREATED="1182983291685" ID="Freemind_Link_127340974" MODIFIED="1182983293111" TEXT="Object">
<node CREATED="1182983347522" ID="Freemind_Link_1468637724" MODIFIED="1182983349463" TEXT="Type">
<node CREATED="1182983357842" ID="Freemind_Link_410341978" MODIFIED="1182983359992" TEXT="File"/>
</node>
<node CREATED="1182983349932" ID="Freemind_Link_409182677" MODIFIED="1182983351560" TEXT="Secure">
<node CREATED="1182983370091" ID="Freemind_Link_259013009" MODIFIED="1183224216832" TEXT="Root owned and not world writable and not group writable and environment is clean"/>
<node CREATED="1182983391680" ID="Freemind_Link_256086182" MODIFIED="1183224224536" TEXT="Trusted user owned and not world writable and not group writable and environment is clean"/>
</node>
<node CREATED="1182983351852" ID="Freemind_Link_1169490214" MODIFIED="1182983355820" TEXT="Insecure">
<node CREATED="1182983405230" ID="Freemind_Link_1847871870" MODIFIED="1182983408551" TEXT="Every other"/>
</node>
</node>
</node>
<node CREATED="1182979749010" ID="Freemind_Link_9342614" MODIFIED="1182979753026" POSITION="right" TEXT="Improvements">
<node CREATED="1182979753760" ID="Freemind_Link_550809777" MODIFIED="1182984667221" TEXT="IBM Paper and Code on TPE doesn&apos;t handle one fundamental case: ie. LD_PRELOAD=&quot;./evil.so&quot; /bin/ls"/>
<node CREATED="1182979809481" ID="Freemind_Link_1682963344" MODIFIED="1182979827931" TEXT="TPE attempts to be modular and support easy addition of future checks"/>
<node CREATED="1182979831199" ID="Freemind_Link_791847569" MODIFIED="1182980180143" TEXT="TPE attempts to be robust and fast since it is called at every do_execve"/>
</node>
<node CREATED="1182980058986" ID="Freemind_Link_159732312" MODIFIED="1182980061761" POSITION="right" TEXT="Challenges">
<node CREATED="1182980062686" ID="Freemind_Link_1004727388" MODIFIED="1182980074808" TEXT="How to handle web CGI execution?">
<node CREATED="1182980101716" ID="Freemind_Link_1032648797" MODIFIED="1182980108406" TEXT="Add binary path to allow list?"/>
</node>
<node CREATED="1183369943768" ID="Freemind_Link_767782384" MODIFIED="1183369965531" TEXT="How to check environment pages for LD_* variables before do_execve?"/>
</node>
<node CREATED="1182986035749" ID="Freemind_Link_294263001" MODIFIED="1182986038912" POSITION="left" TEXT="References">
<node CREATED="1182986040579" ID="Freemind_Link_784186342" MODIFIED="1182988126701" TEXT="IBM Paper on TPE"/>
<node CREATED="1182986045119" ID="Freemind_Link_1696121139" MODIFIED="1182986054515" TEXT="Phrack 56: Trusted Path Execution"/>
<node CREATED="1182986055119" ID="Freemind_Link_911406461" MODIFIED="1182986068249" TEXT="Linux Secure Programming How To: Trusted Path"/>
<node CREATED="1182986069209" ID="Freemind_Link_814977393" MODIFIED="1182986076758" TEXT="TLDP: Linux Kernel Module Programming"/>
<node CREATED="1182988000604" ID="Freemind_Link_419450509" MODIFIED="1182988006667" TEXT="Usenix paper on LSM"/>
</node>
</node>
</map>
