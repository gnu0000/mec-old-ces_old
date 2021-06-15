   { 
   while (getline > 0) 
      { 
      if ($0 == ":userdoc.") 
         break 
      }
   print $0
   while (getline > 0)
      {
      if ($1 != "#line")
         print $0
      }
   }
