ifdef(DEPTH, ,`define(DEPTH, 0)')dnl
define(`BACKPATH', `forloop(`p',`0',DEPTH, `../')')
define(`LINK', `<a href="/~psionic/new`'BACKPATH()$1">$2</a>')dnl

<div id="leftbar">
<div id="links">
	<h3> Navigation </h3>
	<ul>
		<li class="topic"> LINK(/, Home) </li>
		<li> LINK(/about.html, About) </li>
		<li> LINK(/resume.xml, Resume) </li>
		<li> LINK(/entries, Archives) </li>
		<li> LINK(/cvsweb/cvsweb.cgi/, CVSWeb) </li>

		<li class="topic">
			LINK(/articles, Articles)
			<ul>
				<li> Test? </li>
			</ul>
		</li>

		<li class="topic">
			LINK(/projects, Projects)
			<ul>
				<li> Test two? </li>
			</ul>
		</li>

	</ul>
</div>
</div>
