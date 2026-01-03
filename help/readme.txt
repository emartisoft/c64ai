Save help.txt using Windows CRLF line endings and ANSI encoding. 
Each line must not exceed 33 characters. 
Then, create the help file by inverting the letter case 
(convert uppercase to lowercase and lowercase to uppercase) 
and save it. Finally, change all CRLF line endings to LFCR.

receivedLineCount = <line count>;

In m_help function:
...
if (load(m_filename, avaliableDev)==0xff)
{
    receivedLineCount = 66; //  <====== line count
    m_cls();
    writeAIText(0);
    pagecursor = 0;
    showPageNumber();
}
...