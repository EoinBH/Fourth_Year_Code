library(ggplot2);
library(gganimate);
library(gifski);

plot_sens_on_iter <- function(File, Title, maxIt, Grouping=1, GroupStyle="mid") {
  AllVals <- read.table(File,header=T);
  ItVals = AllVals[which(AllVals$It==0),];
  yrs = ItVals$Year;
  numSen <- (length(names(ItVals)) - 2);
  yrRan <- length(yrs);
  senses <- c();

  for (i in 1:numSen) {
    senses <- append(senses, names(ItVals)[i+2]);
  }

  num <- c();
  prob  <- c();
  iteration <- c();
  dataPerIt <- yrRan * numSen;

  for (curr_it in 0:maxIt) {
    ItVals = AllVals[which(AllVals$It==curr_it),];
    it <- c();
    for(i in c(1:dataPerIt)) {
      it <- append(it, c(curr_it));
    }
    for (i in 1:numSen) {
      curr_sense <- c();
      for (j in 1:yrRan) {
        curr_sense <- append(curr_sense, senses[i]);
      }
      num <- append(num, curr_sense);
      prob <- append(prob, ItVals[[i+2]]);
    }
    iteration <- append(iteration, c(it));
  }

  Plot_Data_Frame <- data.frame(
    Sense_Number = num,
    Probability = prob,
    Year = yrs,
    Iteration_Number = iteration
  );

  Final_Plot <- ggplot(Plot_Data_Frame,
  aes(Year, Probability, group = Sense_Number, color = Sense_Number)) +
  geom_line() +
  geom_point() +
  scale_color_viridis_d() +
  labs(x = "Year", y = "Sense Probabilities") +
  theme(legend.position = "bottom") +
  xlim(min(yrs), max(yrs)) +
  ylim(0, 1) +
  transition_time(Iteration_Number) +
  ggtitle(Title)
  #geom_point(aes(group = seq_along(yrs_tmp))) +
  #transition_reveal(yrs_tmp)
  animate(Final_Plot);
  str1 = "./Graphs/animation";
  str2 = as.character(Title);
  str3 = as.character(numSen);
  str4 = "sens";
  str5 = as.character(maxIt);
  str6 = "iter.gif";
  output_filename <- paste(str1,str2,str3,str4,str5,str6,sep="-");
  anim_save(output_filename);
}