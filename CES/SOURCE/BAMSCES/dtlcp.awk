   { 
   while (getline > 0) 
      { 
      if ($0 == "<!START>") 
         break 
      }
   print $0
   while (getline > 0)
      {
      if ($1 != "#line")
         print $0
      }
   }
