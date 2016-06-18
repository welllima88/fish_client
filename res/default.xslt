<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="yes"/>

<xsl:variable name="imagedir">	
	<xsl:value-of select="//channelinfo/@imagedir"/>
</xsl:variable>

<xsl:template match="/">
	<html>
		<head>
		<xsl:if test="./postlist/@type != 'paper'">
			<xsl:call-template name="embedded_script" />
		</xsl:if>
			<xsl:call-template name="embedded_style" />
			<title></title>
		</head>
		<body>
			<xsl:apply-templates/>
		</body>
	</html>
</xsl:template>

<xsl:template match="postlist[@type='list']">
	<xsl:for-each select="postitem">
		<xsl:variable name="postid">
			<xsl:value-of select='./@id'/>
		</xsl:variable>
		<div class="item">
			<table class="item-title-table">
				<tr>
					<td class="td_head">
						<xsl:attribute name="background">
							<xsl:value-of select="concat($imagedir, '/title_head.bmp')"/>					
						</xsl:attribute>			
					</td>
					<td class="td_body">
						<a>
							<xsl:attribute name="class">
								<xsl:text>title</xsl:text>
							</xsl:attribute>
							<xsl:attribute name="href">							
								<xsl:value-of select="concat('javascript:readpost(', string($postid),')')"/>
							</xsl:attribute>
							<xsl:value-of select='./@title' disable-output-escaping="yes"/>
						</a>
					</td>
					<td class="td_tail">
						<xsl:attribute name="background">
							<xsl:value-of select="concat($imagedir, '/title_tail.bmp')"/>
						</xsl:attribute>					
					</td>
				</tr>
			</table>			
			<div class="item-desc">
				<xsl:value-of select='./@desc' disable-output-escaping="yes"/>
				<!--			
				<xsl:text>...</xsl:text>
				<a class="more">
					<xsl:attribute name="onclick">
						<xsl:value-of select="concat('readpost(', string($postid),')')"/>
					</xsl:attribute>
					<xsl:attribute name="onmouseover">
						<xsl:text>style.cursor='pointer'</xsl:text>
					</xsl:attribute>
					<xsl:text>more</xsl:text>
				</a>
				-->
			</div>
			<div class="item-tail">
				<xsl:text>ÃâÃ³ : </xsl:text>
				<a>
					<xsl:attribute name="onclick">
						<xsl:value-of select="concat('readon(', string($postid),')')"/>					
					</xsl:attribute>
					<xsl:choose>
						<xsl:when test="string-length(./@url) &gt; 0">
							<xsl:attribute name="href">
								<xsl:value-of select='./@url'/>			
							</xsl:attribute>
							<xsl:value-of select='./@url'/>					
						</xsl:when>
						<xsl:otherwise>
							<xsl:text>None</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</a>
			<xsl:if test="./@scrap &gt; 0">
				<xsl:text>½ºÅ©·¦ : </xsl:text>
				<xsl:value-of select='./@scrap'/>
			</xsl:if>
			</div>
		</div>
	</xsl:for-each>
</xsl:template>


<xsl:template match="postlist[@type='post']">
	<xsl:for-each select="postitem">
			<xsl:variable name="postid">
			<xsl:value-of select='./@id'/>
		</xsl:variable>
		<div class="item">
			<div class="title">
				<table class="item-title-table">
					<tr>
						<td class="td_head">
							<xsl:attribute name="background">
								<xsl:value-of select="concat($imagedir, '/title_head.bmp')"/>					
							</xsl:attribute>
						</td>
						<td class="td_body">							
							<xsl:choose>
								<xsl:when test="string-length(./@url) &gt; 0">
									<a>
										<xsl:attribute name="class">
											<xsl:text>title</xsl:text>
										</xsl:attribute>
										<xsl:attribute name="onclick">
											<xsl:value-of select="concat('readon(', string($postid),')')"/>
										</xsl:attribute>
										<xsl:attribute name="href">
											<xsl:value-of select='./@url'/>			
										</xsl:attribute>														
										<xsl:value-of select='./@title' disable-output-escaping="yes"/>
									</a>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select='./@title' disable-output-escaping="yes"/>
								</xsl:otherwise>
							</xsl:choose>							
						</td>
						<td class="td_tail">
							<xsl:attribute name="background">
								<xsl:value-of select="concat($imagedir, '/title_tail.bmp')"/>
							</xsl:attribute>
						</td>
					</tr>
				</table>
			</div>
			<div class="item-desc">
				<xsl:value-of select='./@desc' disable-output-escaping="yes"/>
			</div>
			<div class="item-tail">
				<xsl:text>ÃâÃ³ : </xsl:text>
				<a>
					<xsl:attribute name="onclick">
						<xsl:value-of select="concat('readon(', string($postid),')')"/>
					</xsl:attribute>
					<xsl:choose>
						<xsl:when test="string-length(./@url) &gt; 0">
							<xsl:attribute name="href">
								<xsl:value-of select='./@url'/>			
							</xsl:attribute>
							<xsl:value-of select='./@url'/>					
						</xsl:when>
						<xsl:otherwise>
							<xsl:text>None</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</a>
				<a class="scrap">
					<xsl:attribute name="onmouseover">
						<xsl:text>style.cursor='pointer'</xsl:text>
					</xsl:attribute>
					<xsl:attribute name="onclick">
						<xsl:value-of select="concat('scrap(', string($postid),')')"/>
					</xsl:attribute>
					<xsl:text>½ºÅ©·¦</xsl:text>
				</a>
			</div>		
		</div>
	</xsl:for-each>
</xsl:template>


<xsl:template match="postlist[@type='paper']">
	<xsl:for-each select="postitem">
			<xsl:variable name="postid">
			<xsl:value-of select='./@id'/>
		</xsl:variable>
		<div class="item">
			<div class="title">
				<table class="item-title-table">
					<tr>
						<td class="td_head">
							<xsl:attribute name="background">
								<xsl:value-of select="concat($imagedir, '/title_head.bmp')"/>
							</xsl:attribute>
						</td>
						<td class="td_body">							
							<xsl:choose>
								<xsl:when test="string-length(./@url) &gt; 0">
									<a>
										<xsl:attribute name="class">
											<xsl:text>title</xsl:text>
										</xsl:attribute>										
										<xsl:attribute name="href">
											<xsl:value-of select='./@url'/>			
										</xsl:attribute>														
										<xsl:value-of select='./@title' disable-output-escaping="yes"/>
									</a>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select='./@title' disable-output-escaping="yes"/>
								</xsl:otherwise>
							</xsl:choose>							
						</td>
						<td class="td_tail">
							<xsl:attribute name="background">
								<xsl:value-of select="concat($imagedir, '/title_tail.bmp')"/>
							</xsl:attribute>
						</td>
					</tr>
				</table>
			</div>
			<div class="item-desc">
				<xsl:value-of select='./@desc' disable-output-escaping="yes"/>
			</div>
			<div class="item-tail">
				<xsl:text>ÃâÃ³ : </xsl:text>
				<a>					
					<xsl:choose>
						<xsl:when test="string-length(./@url) &gt; 0">
							<xsl:attribute name="href">
								<xsl:value-of select='./@url'/>			
							</xsl:attribute>
							<xsl:value-of select='./@url'/>					
						</xsl:when>
						<xsl:otherwise>
							<xsl:text>None</xsl:text>
						</xsl:otherwise>
					</xsl:choose>
				</a>				
			</div>		
		</div>
	</xsl:for-each>
</xsl:template>


<xsl:template name="embedded_script">
<script>
	function swap(img)
	{
		var oldSrc = img.src; 
		var folderEnd = oldSrc.lastIndexOf('/') + 1; 

		if(img.useMap.indexOf('#readstate')==0)
		{
			if(oldSrc.indexOf('unread.gif')== -1)
			{
				img.src = oldSrc.substr(0, folderEnd) + 'unread.gif'; 
			}
			else
			{
				img.src = oldSrc.substr(0, folderEnd) + 'read.gif'; 
			}
		}
	}

	function readon(id)
	{
		//swap(img);
		window.external.ReadOn(id);
		//window.external.ReadOn(String(id));		
	}

	function readpost(id)
	{
		window.external.ReadPost(id);
	}
	
	function scrap(id)
	{
		window.external.Scrap(id);	
	}
</script>
</xsl:template>

<xsl:template name="embedded_style">
<style type="text/css">
	body {
		background-color: #FFFFFF;
		margin: 0px;
		padding: 14px;			
		border-top: 5px #768dad solid;
		border-left: 5px #768dad solid;
		border-bottom: 5px #768dad solid;
		border-right: 5px #768dad solid;

		font-family: Dotum, verdana, arial, sans-serif;
		color: #414141
	}
	
	a, a:visited, a:active { 
		color: #355EA0;
		text-decoration: none;		
	}
	
	a:hover {
		color: #FF6600;
		text-decoration: none;		
	}


	table.item-title-table {
		border-collapse: collapse;
		border: 0px;
		margin: 0px;
		padding: 0px;
		height: 35px;
		width: 100%;	

		margin: 0px 0px 10px
	}

	table.item-title-table td.td_head {	
		padding: 0px;
		width: 13px
	}

	table.item-title-table td.td_body {
		text-align: left;
		padding: 0px;
		background-color: #dadada
	}

	table.item-title-table td.td_tail {
		padding: 0px;
		width: 85px
	}
	
	a:link.title {
		color: #414141;
		font-size: 12pt;
		font-weight: bold;
		text-decoration: none
	}

	a:visited.title {
		color: #414141;
		font-size: 12pt;
		font-weight: bold;
		text-decoration: none
	}

	a:hover.title {
		color: #149da2;
		font-size: 12pt;
		font-weight: bold;
		text-decoration: none
	}

	a:active.title {
		color: #414141;
		font-size: 12pt;
		font-weight: bold;
		text-decoration: none
	}

	div.item-desc {
		font-size: 11pt;
		color: #414141;
		margin: 15px 5px 0px 5px
	}

	div.item-tail {
		border-top: 1px dotted #CBCBCB;
		text-align: left;
		font-size: 11pt;
		color: #999999;
		margin: 10px 0px 30px 0px;
		padding: 10px 0px 10px 0px
		
	}

	div.item-tail a:link {
		color: #999999;
		font-size: 11pt;
		font-weight: normal;
		text-decoration: none;

		margin: 0px 30px 0px 0px
	}

	div.item-tail a:visited {
		color: #999999;
		font-size: 11pt;
		font-weight: normal;
		text-decoration: none;

		margin: 0px 30px 0px 0px
	}

	div.item-tail a:hover{		
		color: #414141;
		font-size: 11pt;
		font-weight: normal;

		text-decoration: none
	}

	div.item-tail a:active{
		color: #999999;
		font-size: 11pt;
		font-weight: normal;
		text-decoration: none		
	}

	div.item-tail a.scrap {	
		align: right;
		color: #999999;
		font-size: 11pt;
		font-weight: normal;
		text-decoration: none;

		margin: 0px 0px 0px 0px
	}

</style>
</xsl:template>
</xsl:stylesheet>
