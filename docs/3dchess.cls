\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{3dchess}
\LoadClass[
  11pt, 
  a4paper
]{article}
\bibliographystyle{plain}

\RequirePackage[utf8]{inputenc} % direct input of unicode chars
\RequirePackage{cmap} % create searchable PDF files
\RequirePackage[T1]{fontenc}
\RequirePackage{lmodern} % latin modern font type
\RequirePackage{microtype}
\RequirePackage[ngerman]{babel}

\RequirePackage{geometry}
\geometry{a4paper,left=25mm,right=25mm, top=30mm, bottom=30mm} 

\RequirePackage{fancyhdr}
\pagestyle{fancy}
\lhead{\@author}
\chead{}
\rhead{\nouppercase{\leftmark}}
\lfoot{}
\cfoot{}
\rfoot{\thepage}
\renewcommand{\headrulewidth}{0.4pt}
%\renewcommand{\footrulewidth}{0.4pt}
\setlength{\footnotesep}{10pt}
\setlength{\skip \footins}{14pt}
\setlength{\headheight}{14pt}
\setlength{\parindent}{5mm}

\RequirePackage[
  colorlinks=false,
  %linkcolor=black,
  %citecolor=black,
  %filecolor=black,
  %urlcolor=black,
  bookmarks=true,
  bookmarksopen=true, 
  bookmarksopenlevel=2,
  plainpages=false,
  pdfpagelabels=true
]{hyperref}
\RequirePackage{graphicx}
\RequirePackage{wrapfig}
\RequirePackage{mathtools}

\RequirePackage{listings}
\RequirePackage{color}
\definecolor{lightgray}{rgb}{0.9,0.9,0.9}
\definecolor{commentgreen}{rgb}{0.45,0.65,0.35}
\definecolor{stringred}{rgb}{0.75,0.05,0.05}
\definecolor{pink}{rgb}{0.5,0,1}
\lstset{
  language=C++,
  basicstyle=\ttfamily,
  backgroundcolor=\color{lightgray},
  identifierstyle=\color{black},
  keywordstyle=\color{blue},
  stringstyle=\color{stringred},
  showstringspaces=false,
  commentstyle=\color{commentgreen},
  %morecomment=[l][\color{magenta}]{\#,},
  %breakatwhitespace=true,
  breaklines=true,
  tabsize=4,
  numbers=left,
  numberstyle=\tiny,
  numbersep=5pt, 
  xleftmargin=8pt,
  xrightmargin=3pt
}



\newcommand{\university}{Hochschule Karlsruhe -- Technik und Wirtschaft (HsKA)}
\newcommand{\faculty}{Fakultät für Informatik und Wirtschaftsinformatik}
\newcommand{\institute}{Fachbereich Informatik}
\newcommand{\course}{Studiengang Informatik (M. Sc.)}
\newcommand{\worktype}{Projektarbeit (Master)}
\newcommand{\professor}{Prof. Dr. Christian Pape}
\newcommand{\workdate}{Wintersemester 2013/2014}
\newcommand{\authormat}[1]{\long\def\@authormat{#1}}

\renewcommand{\maketitle}{
  \begin{titlepage}
	\thispagestyle{empty}
  	\begin{center}
  	  {\large\bfseries \university} \\[2mm]
	  {\large\bfseries \faculty} \\[2mm]
  	  {\large \institute} \\[2mm]
	  \vfill\vfill
	  {\huge\bfseries \@title\par}
	  \vfill
	  {\Large\worktype} \\
	  \vfill
	  von \\
	  \vfill
	  {\Large\bfseries \@author~(\@authormat)} \\[2mm]
	  {\large\course } \\
	  \vfill\vfill
	  \begin{tabular}{lll}
		%\textbf{Verfasser:} & \@author~(\@authormat) \\[0.2cm]
		%\textbf{Datum:} & \today \\[0.2cm]
		\textbf{Bearbeitungszeit:} & \workdate \\[2mm]
		\textbf{Betreuer:} & \professor
	  \end{tabular}
	\end{center}
  \end{titlepage}
}