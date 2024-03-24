filename = "roce.txt"

class qm_info:
  def __init__(self, name, inq, outq) -> None:
    self.name = name
    self.inq = inq
    self.outq = outq

  def print_data(self):
    print(self.name)
    print(self.inq)
    print(self.outq)


class parse_info:
  def __init__(self, filename) -> None:
    self.filename = filename

    # reading_state is an FSM state indicator
    # 1 -> reading input queues
    # 2 -> reading output queues
    # 3 -> other info
    # 4 -> queueing modules
    self.reading_state = 0
    self.data = []
    self.nodes = set()
    self.edges = {}


  def parse_qm_line(self, qms : list) -> dict:
    qms = { i.split(":")[0]:i.split(":")[1] for i in qms if len(i.split(":")) == 2}
    return qms

# 
  def parse_qm(self, qm : list) -> qm_info:
    input_index = qm.index("input queues:")
    output_index = qm.index("output queues:")

    name = qm[0]
    self.nodes.add(name)

    inqs = [qm[i] for i in range(input_index+1, output_index)]
    outqs = [qm[i] for i in range(output_index+1, len(qm))]
    return qm_info(name, self.parse_qm_line(inqs), self.parse_qm_line(outqs))

  def parse_file(self):
    f = open(self.filename)
    lines = [i.strip() for i in f.readlines()]
    raeding_state = 0
    inqs = []
    outqs = []
    other = []
    qms = []
    temp_qm = []
    reading_state = 0
    keys = ["input queues", "output queues", "other info", "queueing modules"]
    for line in lines:
      # Handle FSM transitinos
      if reading_state == 4 and "---------- T =" in line:
        if len(temp_qm) > 0:
          qms.append(self.parse_qm(temp_qm))
        reading_state = 0
        values = [inqs, outqs, other, qms]
        new_data = dict(zip(keys, values))
        self.data.append(new_data)
        inqs = []
        outqs = []
        other = []
        qms = []
        temp_qm = []
        continue
      elif reading_state == 0 and "input queues" in line:
        reading_state = 1
        continue
      elif reading_state == 1 and "output queues" in line:
        reading_state = 2
        continue
      elif reading_state == 2 and "other info" in line:
        reading_state = 3
        continue
      elif reading_state == 3 and "-------------" in line:
        reading_state = 4
        continue
      elif reading_state == 4 and "-------------" in line:
        qms.append(self.parse_qm(temp_qm))
        temp_qm = []
        continue


      if reading_state == 1:
        inqs.append(line)
      elif reading_state == 2:
        outqs.append(line)
      elif reading_state == 3:
        other.append(line)
      elif reading_state == 4:
        temp_qm.append(line)

    
    qms.append(self.parse_qm(temp_qm))
    values = [inqs, outqs, other, qms]
    new_data = dict(zip(keys, values))
    self.data.append(new_data)

  def print_data(self):
    for i in self.data:
      for j in i["queueing modules"]:
        # print((j))
        j.print_data()
        # print()
        pass
  
  def print_nodes(self):
    for i in self.nodes:
      print(i)
      
      # print(i[0])
    # print(self.data)

a = parse_info(filename)
a.parse_file()
# a.print_data()
a.print_nodes()
