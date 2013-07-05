<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
  <h2>TarotClub deal summary</h2>
    <h4>Taker: <xsl:value-of select="deal_result/deal_info/taker" /></h4>
    <h4>Contract: <xsl:value-of select="deal_result/deal_info/contract" /></h4>
    <h4>First player: <xsl:value-of select="deal_result/deal_info/first_trick_lead" /></h4>
    <h4>Discard: <xsl:value-of select="deal_result/discard" /></h4>  

    <table border="1">
      <tr bgcolor="#9acd32">
        <th>Turn</th>
        <th>South</th>
	<th>East</th>
        <th>North</th>
	<th>West</th>
      </tr>
      <xsl:for-each select="deal_result/tricks/trick">
      <tr>
        <td><xsl:value-of select="@number"/></td>
	<xsl:for-each select="card">
	    <td><xsl:value-of select="."/></td>
	</xsl:for-each>
      </tr>
      </xsl:for-each>
    </table>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>


