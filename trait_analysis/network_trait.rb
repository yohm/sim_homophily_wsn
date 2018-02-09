require 'json'

class Network

  def initialize
    @links = {}
  end

  def add_link(n1,n2,w)
    @links[n1] ||= {}
    @links[n1][n2] = w
    @links[n2] ||= {}
    @links[n2][n1] = w
  end

  def nodes
    @links.keys
  end

  def self.load_net_file(f)
    net = self.new
    File.open(f).each do |line|
      a = line.chomp.split
      net.add_link(a[0].to_i,a[1].to_i,a[2].to_f)
    end
    net
  end

  def ego_net(ego)
    ns = [ego]
    ns += @links[ego].keys
    net = self.class.new
    @links[ego]
    ns.each do |n|
      @links[n].each do |n1,w|
        if ns.include?(n1)
          net.add_link(n,n1,w)
        end
      end
    end
    net
  end

  def for_each_link
    @links.each_pair do |n1,n2_w|
      n2_w.each_pair do |n2,w|
        yield n1,n2,w if n1 < n2
      end
    end
  end

  def remap_index
    id_map = Hash[nodes.map.with_index {|n,i| [n,i]}]
    new_net = self.class.new
    for_each_link {|n1,n2,w| new_net.add_link(id_map[n1],id_map[n2],w) }
    new_net
  end

  def to_json
    @links.to_json
  end
end

class NetworkTrait

  def initialize(net,traits)
    @net = net
    @traits = traits
  end

  attr_reader :net, :traits

  def self.load(net_file, trait_file)
    net = Network.load_net_file(net_file)
    traits = {}
    File.open(trait_file).each_with_index do |line,i|
      traits[i] = line.chomp.split.map(&:to_i)
    end
    self.new(net,traits)
  end

  def self.load_from_dir(dir)
    self.load( File.join(dir, 'net.edg'), File.join(dir, 'traits.txt') )
  end

  def ego_net(n)
    new_net = @net.ego_net(n)
    id_map = Hash[new_net.nodes.map.with_index {|n,i| [n,i] }]
    new_traits = {}
    id_map.each do |n,i|
      new_traits[i] = @traits[n]
    end
    self.class.new(new_net.remap_index, new_traits)
  end

  def normalize_traits(n=0)
    q = @traits.values.flatten.max + 1
    base = @traits[n]
    new_traits = {}
    @traits.each_pair do |i,a|
      new_traits[i] = a.zip(base).map do |x,b|
        (x-b)%q
      end
    end
    @traits = new_traits
  end

  def print_traits(io=$stdout)
    @traits.each do |i,a|
      mapped = a.map {|x| x==0 ? "\033[41m#{x}\033[0m" : x.to_s}
      io.puts("#{sprintf('%2d',i)} #{mapped.join(' ')}")
    end
  end
end

if __FILE__ == $0
  n = NetworkTrait.load_from_dir(ARGV[0])
  f = JSON.parse(File.open('_input.json').read)['simulation_parameters']['F']
  histo = Hash.new(0)
  n.net.for_each_link do |n1,n2,w|
    t1 = n.traits[n1]
    t2 = n.traits[n2]
    overlap = t1.zip(t2).count {|x,y| x==y}
    histo[overlap] += 1
  end
  sum = histo.values.inject(:+)
  histo.each_pair {|k,v| histo[k] = v.to_f/sum }
  avg = histo.map {|k,v| k*v}.inject(:+)
  File.open('_output.json','w') {|io| JSON.dump({overlap:avg,overlap_ratio:avg/f},io)}
  keys = (1..f).to_a
  values = keys.map {|k| histo[k]}
  require 'pycall'
  plt = PyCall.import_module('matplotlib.pyplot')
  plt.plot(keys, values)
  plt.savefig("overlap_distribution.png")
end
