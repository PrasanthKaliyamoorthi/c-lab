The official IETF standard specification for CSV files and CSV parsers is **RFC 4180**.

---

### **Primary RFC: RFC 4180**
* **Title:** Common Format and MIME Type for Comma-Separated Values (CSV) Files
* **MIME Type:** `text/csv`
* **Status:** Informational (De facto standard specification)

---

### **Key Rules for Building an RFC 4180-Compliant CSV Parser**

1. **Record Delimiter:**
   * Each record is located on a separate line, delimited by a line break (`CRLF` / `\r\n`).
   * *Parser Note:* Robust parsers usually handle `\n` or `\r\n` transparently.

2. **Field Separator:**
   * Fields are delimited by commas (`,`).
   * The last field in a record **must not** have a trailing comma.

3. **Optional Header:**
   * The first line may be a header row matching the field structure of subsequent records.
   * MIME parameter: `text/csv; header=present` or `header=absent`.

4. **Quoted Fields (`"`):**
   * A field **must** be enclosed in double quotes if it contains:
     * Commas (`,`)
     * Line breaks (`\r` or `\n`)
     * Double quotes (`"`)
   * Inside a quoted field, a literal double quote **must** be escaped by doubling it (`""`).
     * *Example:* `"Hello ""World"""` $\rightarrow$ `Hello "World"`

5. **Spaces and Whitespace:**
   * Spaces are considered part of the field string and **must not** be trimmed automatically by default.

6. **Consistent Column Counts:**
   * Every record in the file should contain the exact same number of fields.

---

### **Related RFC**
* **RFC 7111:** *URI Fragment Identifiers for the text/csv Media Type* (defines row/column range selections like `#row=5-10` or `#col=2` in CSV URIs).
