<img align="left" src="https://github.com/xastrix/cmap/blob/master/logo.png" width="180">
<b>cmap</b>
<p>A micro version control system written in C++ and designed for tracking changes in files and managing project versions efficiently.</p>
<p>This project is not finalized and may have bugs, so please report them to <a href="https://github.com/xastrix/cmap/issues">Issues</a></p>
<p>
  <a href="#features">Features</a> •
  <a href="#installing">Installing</a> •
  <a href="#dependencies">Dependencies</a>
</p>
<h2>Features</h2>
<ul>
  <li>Initialization of a repository similar to git</li>
  <li>Adding files to the list of tracked files</li>
  <li>Committing to save the current state of your tracked files</li>
  <li>Viewing the status of the repository (including modified, deleted, and untracked files)</li>
  <li>Viewing logs (including hash, commit message, datetime, and committer information)</li>
</ul>
<h2>Installing</h2>
<p>Clone the repository</p>
<pre>git clone --recursive https://github.com/xastrix/cmap.git</pre>
<p>Generate VS project by premake5</p>
<pre>cd cmap && premake5 vs20xx</pre>
<p>
  Launch <kbd>cmap.sln</kbd> (Visual Studio Solution File) and compile project by pressing <kbd><kbd>CTRL</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd></kbd>
</p>
<h3>Requirements</h3>
<ul>
  <li>Visual Studio</li>
  <li>Windows SDK</li>
  <li><a href="https://github.com/premake/premake-core">premake</a></li>
</ul>
<h2>Dependencies</h2>
<a href="https://github.com/open-source-parsers/jsoncpp">jsoncpp</a>