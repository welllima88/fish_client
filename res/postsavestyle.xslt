<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="yes"/>

<xsl:variable name="imagedir">	
	<xsl:value-of select="//channelinfo/@imagedir"/>
</xsl:variable>

<xsl:template match="/">
	<html>
		<head>
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
			<div class="title">
				<a> 
					<xsl:attribute name="class">
						<xsl:text>item-title</xsl:text>
					</xsl:attribute>
					<xsl:attribute name="href">
						<xsl:value-of select='./@url'/>
					</xsl:attribute>
					<xsl:value-of select='./@title' disable-output-escaping="yes"/>
				</a>
				
				<table class="bar"><tr><td></td></tr></table>			
			</div>
			<div class="item-desc">
				<xsl:value-of select='./@desc' disable-output-escaping="yes"/>				
			</div>
			<div class="item-tail">
				<xsl:text>출처 : </xsl:text>
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select='./@url'/>			
					</xsl:attribute>
					<xsl:value-of select='./@url'/>					
				</a>
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
				<a> 
					<xsl:attribute name="class">
						<xsl:text>item-title</xsl:text>
					</xsl:attribute>
					<xsl:attribute name="href">
						<xsl:value-of select='./@url'/>
					</xsl:attribute>
					<xsl:value-of select='./@title' disable-output-escaping="yes"/>
				</a>
				
				<table class="bar"><tr><td></td></tr></table>			
			</div>
			<div class="item-desc">
				<xsl:value-of select='./@desc' disable-output-escaping="yes"/>				
			</div>
			<div class="item-tail">
				<xsl:text>출처 : </xsl:text>
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select='./@url'/>			
					</xsl:attribute>
					<xsl:value-of select='./@url'/>					
				</a>
			</div>
		</div>
	</xsl:for-each>
</xsl:template>

<xsl:template name="embedded_style">
<style type="text/css">
	body {
		background-color: #FFFFFF;
		margin: 0px;
		padding: 0px;			
		border-top: 5px #768dad solid;
		border-left: 5px #768dad solid;
		border-bottom: 5px #768dad solid;

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

	div.title {
		background-color : #FFFFFF;
		valign: bottom;
		width: 100%;
		margin: 10px 13px 19px 13px;
	}

	
	a:link.item-title {
		color: #051CA4;
		font-size: 20pt;
		font-weight: bold;
		text-decoration: none
	}

	a:visited.item-title {
		color: #051CA4;
		font-size: 20pt;
		font-weight: bold;
		text-decoration: none
	}

	a:hover.item-title {
		color: #6778DA;
		font-size: 20pt;
		font-weight: bold;
		text-decoration: none
	}

	a:active.item-title {
		color: #051CA4;
		font-size: 20pt;
		font-weight: bold;
		text-decoration: none
	}
	
	table.title-table {
		width: 100%;
		vertical-align: bottom;
		margin: 0px 0px 0px 0px
	}

	table.bar {
		width: 100%;
		height: 6px;
		border-top: 1px solid #AAADC3;
		background-color: #E9EBF7;
		margin: 8px 0px 0px 0px
	}
		

	div.item-desc {
		font-size: 11pt;
		color: #380000;
		margin: 15px 13px 5px 13px
	}

	div.item-tail {		
		text-align: left;
		font-size: 10pt;
		color: #999999;
		margin: 0px 13px 60px 13px;
		padding: 0px 0px 0px 0px
		
	}

	div.item-tail a:link {
		color: #999999;
		font-size: 10pt;
		font-weight: normal;
		text-decoration: none;

		margin: 0px 15px 0px 0px
	}

	div.item-tail a:visited {
		color: #999999;
		font-size: 10pt;
		font-weight: normal;
		text-decoration: none;

		margin: 0px 15px 0px 0px
	}

	div.item-tail a:hover{		
		color: #414141;
		font-size: 10pt;
		font-weight: normal;

		text-decoration: none
	}

	div.item-tail a:active{
		color: #999999;
		font-size: 10pt;
		font-weight: normal;
		text-decoration: none		
	}

	div.item-tail a.scrap {	
		align: right;
		color: #999999;
		font-size: 10pt;
		font-weight: normal;
		text-decoration: none;

		margin: 0px 0px 0px 0px
	}

</style>
</xsl:template>
</xsl:stylesheet>
