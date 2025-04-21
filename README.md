<img align="left" src="https://github.com/xastrix/cmap/blob/master/media/logo.png" width="180">
<h4>cmap</h4>
<p>A micro version control system written in C++ and designed for tracking changes in files and managing project versions efficiently.</p>
<br>
<p>
  <a href="#features">Features</a> •
  <a href="#usage">Usage</a> •
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
<h2>Usage</h2>
<pre>cmap h/help</pre>
<p>First, you need to set the user settings</p>
<pre>cmap config "username" "email" --user-config</pre>
<p>Initialize the repository</p>
<pre>cmap i/init</pre>
<p>Add files to the tracked files with the command</p>
<pre>cmap add "filename"</pre>
<p>To remove an accidentally included file, use the command</p>
<pre>cmap add "filename" --rm</pre>
<p>To check the tracked, deleted, or modified files, use the command</p>
<pre>cmap status</pre>
<p>This command rolls back the specified commit identified by the provided hash</p>
<pre>cmap u/undo "hash"</pre>
<p>To create your first commit, type</p>
<pre>cmap commit "message"</pre>
<p>Show commit list (including hash, commit message, datetime, and committer information)</p>
<pre>cmap log</pre>
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